#include "GeneticTest.h"

#include "Board.h"
#include "Player.h"
#include "HandEvaluator.h"

GeneticTest::GeneticTest()
{
	Evaluator = new HandEvaluator();
	TestBoard = new Board(Evaluator, 20, false);
	TestBoard->SetActive(true);
}

GeneticTest::~GeneticTest()
{

}

void GeneticTest::Start()
{
	InitializePopulation(PopulationSize);
	std::cout << "Populaton of " << Population.size() << " initialized: " << GetPopulationStr() << "\n\n";
}

void GeneticTest::Update()
{
	std::cout << "Generation " << Generation << ":\n";

	GenerateSubjects(PopulationSize);
	std::cout << RandomSubjects.size() << " subjects generated...\n";

	MeasureFitness();	
	PrintPopulationFitness();

	if (!IsTestComplete())
		ReproducePopulation();
	else
		End();
}

void GeneticTest::End()
{

}

bool GeneticTest::IsTestComplete()
{
	return GetOverallFitness() >= TargetFitness ? true : false;
}

void GeneticTest::InitializePopulation(unsigned int _Size)
{
	Population.clear();

	for (unsigned int Index = 0; Index < _Size; Index++)
	{
		Population.insert(std::make_pair(new Player(TestBoard, PlayersGenerated), 0.0));
		PlayersGenerated++;
	}
}

void GeneticTest::GenerateSubjects(unsigned int _Size)
{
	RandomSubjects.clear();

	auto Seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	std::mt19937 mt(Seed);
	std::uniform_int_distribution<int> IndexDistribution(0,1000);

	for (unsigned int Index = 0; Index < _Size; Index++)
		RandomSubjects.push_back(new Player(TestBoard, IndexDistribution(mt)));
}

double GeneticTest::DetermineWinRate(Player* _Current, Player* _Subject)
{
	TestBoard->Reset();

	TestBoard->AddPlayer(_Current);
	TestBoard->AddPlayer(_Subject);

	TestBoard->Start();

	//std::cout << "Initial: P." << _Current->GetIndex() << " (S: $" << _Current->GetStack() << "/A: $" << _Current->GetAnte() << ") SP." << _Subject->GetIndex() << " (S: $" << _Subject->GetStack() << "/A: $" << _Subject->GetAnte() << ") Pot ($" << TestBoard->GetPot() << ") \n";

	while (TestBoard->GetRounds() < RoundLimit)
	{
		TestBoard->Update();
		//std::cout << "Round " << TestBoard->GetRounds() << "/" << RoundLimit << " - P." << TestBoard->GetCurrentPlayer()->GetIndex() << "'s turn (" << TestBoard->GetStateStr() << " - $" << TestBoard->GetPot() << ")   \r";

		if (TestBoard->IsGameEnded())
		{
			TestBoard->Reset();

			TestBoard->AddPlayer(_Current);
			TestBoard->AddPlayer(_Subject);

			TestBoard->Start();
		}
	}

	//std::cout << "Result: P." << _Current->GetIndex() << " (S: $" << _Current->GetStack() << "/A: $" << _Current->GetAnte() << ") SP." << _Subject->GetIndex() << " (S: $" << _Subject->GetStack() << "/A: $" << _Subject->GetAnte() << ") Pot ($" << TestBoard->GetPot() << ") = Ended at " << TestBoard->GetStateStr() << "\n";
	//std::cout << "P." << _Current->GetIndex() << " against SP." << _Subject->GetIndex() << ": " << (double)TestBoard->GetPlayerWins(_Current) / (double)TestBoard->GetRounds() << " (Wins: " << TestBoard->GetPlayerWins(_Current) << " | Rounds: " << TestBoard->GetRounds() << ")\n";

	return (double)TestBoard->GetPlayerWins(_Current) / (double)TestBoard->GetRounds();
}

void GeneticTest::MeasureFitness()
{
	Player* CurrentPlayer = nullptr;
	double AverageWinRate = 0.0;
	unsigned int SubjectInd = 0;

	for (auto & Member : Population) //auto PopulationItr = Population.begin(); PopulationItr != Population.end(); ++PopulationItr)
	{
		CurrentPlayer = Member.first;
		AverageWinRate = 0.0;

		for (auto const& Subject : RandomSubjects)
		{
			AverageWinRate += DetermineWinRate(CurrentPlayer, Subject);
			SubjectInd++;

			std::cout << "P." << CurrentPlayer->GetIndex() << " dualing against SP." << Subject->GetIndex() << "... (Subject Done: " << SubjectInd << "/" << RandomSubjects.size() << ")      \r";
		}

		AverageWinRate /= RandomSubjects.size();
		Member.second = AverageWinRate;

		std::cout << "\n";
		SubjectInd = 0;
	}
}

double GeneticTest::GetOverallFitness()
{
	double TotalFitness = 0.0;
	for (auto const& Member : Population)
		TotalFitness += Member.second;

	return TotalFitness / Population.size();
}

bool GeneticTest::HasHigherFitness(std::pair<Player*, double> _First, std::pair<Player*, double> _Second)
{
	return _First.second >= _Second.second ? true : false;
}

bool GeneticTest::HasMutationHappen()
{
	auto Seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	std::mt19937 mt(Seed);
	std::uniform_real_distribution<double> ChanceDistribution(0.0, 1.0);

	return ChanceDistribution(mt) <= MutationRate ? true : false;
}

void GeneticTest::ReproducePopulation()
{
	std::cout << "Reproducing population...\n";

	//Get a copy of current population
	std::vector<std::pair<Player*, double>> PopulationSample(Population.begin(), Population.end());

	Population.clear();

	auto Seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::mt19937 mt(Seed);
	std::uniform_int_distribution<int> QualifierDistribution(0, PopulationSample.size() - 1);
	
	std::vector<std::pair<Player*, double>> Touranment;
	std::pair<Player*, double> CurrentContender;

	//Select Parents from current population via the Tourament algorithm 
	for (unsigned int Index = 0; Index < ParentLimit/WinnerPerTouranment; Index++)
	{
		Touranment.clear();

		for (unsigned int TourIndex = 0; TourIndex < TouramentSize; TourIndex++)
		{
			do 
			{
				CurrentContender = PopulationSample[QualifierDistribution(mt)];
			} 
			while (Population.find(CurrentContender.first) != Population.end());

			Touranment.push_back(CurrentContender);
		}
		
		CurrentContender = Touranment[0];
		for (auto const& Player : Touranment)
		{
			if (Player.second > CurrentContender.second)
				CurrentContender = Player;
		}

		Population.insert(std::make_pair(CurrentContender.first, CurrentContender.second));
		//std::cout << "Parent " << Index << " (P." << CurrentContender.first->GetIndex() << ")\n";//=> " << GetThresholdsStr(CurrentContender.first) << "\n";
	}

	Player* Parent_A = Population.begin()->first;//nullptr;
	Player* Parent_B = (++Population.begin())->first;//nullptr;
	Player* NewChild = nullptr;

	if (Parent_A == nullptr || Parent_B == nullptr)
		std::cout << "Selected Parent is null...\n";
	else if (Parent_A->GetAI() == nullptr || Parent_B->GetAI() == nullptr)
		std::cout << "Selected Parent's AI is null...\n";

	std::array<double, 8> Thresholds_A = Parent_A->GetAI()->GetThresholds();
	std::array<double, 8> Thresholds_B = Parent_B->GetAI()->GetThresholds();

	std::uniform_int_distribution<int> ParentDistribution(0, ParentLimit - 1);
	std::uniform_int_distribution<int> SelectDistribution(0, 1);
	std::uniform_int_distribution<int> ThresholdDistribution(0, 7);
	std::uniform_real_distribution<double> MutationDistribution(0.0, 1.0);

	//Cross-match the candidates within the population to generate a new set of population
	for (unsigned int Index = 0; Index < PopulationSample.size() - ParentLimit; Index++)
	{
		//Two Point Cross-over
		/*std::array<double, 8> CombinedThresholds;
		int Cross_First = ThresholdDistribution(mt);
		int Cross_Second = ThresholdDistribution(mt);

		for (unsigned int ThrIndex = 0; ThrIndex < Cross_First; ThrIndex++)
			CombinedThresholds[ThrIndex] = Thresholds_A[ThrIndex];

		for (unsigned int ThrIndex = Cross_First; ThrIndex < Cross_Second; ThrIndex++)
			CombinedThresholds[ThrIndex] = Thresholds_B[ThrIndex];

		for (unsigned int ThrIndex = Cross_Second; ThrIndex < Thresholds_A.size(); ThrIndex++)
			CombinedThresholds[ThrIndex] = Thresholds_A[ThrIndex];*/

		//Single Point Cross-over
		/*std::array<double, 8> CombinedThresholds;	
		int Cross_Single = ThresholdDistribution(mt);

		for (unsigned int ThrIndex = 0; ThrIndex < Cross_Single; ThrIndex++)
			CombinedThresholds[ThrIndex] = Thresholds_A[ThrIndex];

		for (unsigned int ThrIndex = Cross_Single; ThrIndex < 8; ThrIndex++)
			CombinedThresholds[ThrIndex] = Thresholds_B[ThrIndex];*/

		// Uniform Cross-over
		std::array<double, 8> CombinedThresholds;
		for (unsigned int ThrIndex = 0; ThrIndex < 8; ThrIndex++)
			CombinedThresholds[ThrIndex] = SelectDistribution(mt) == 0 ? Thresholds_A[ThrIndex] : Thresholds_B[ThrIndex];

		//Will mutation happen?
		if (HasMutationHappen())
			CombinedThresholds[ThresholdDistribution(mt)] = MutationDistribution(mt);

		NewChild = new Player(TestBoard, PlayersGenerated, CombinedThresholds);
		Population.insert(std::make_pair(NewChild, 0.0));

		//std::cout << "Child " << Index << ": P." << PlayersGenerated << " (" << GetThresholdsStr(NewChild) << ")\n";
	
		PlayersGenerated++;
	}
	
	Generation++;
}

std::string GeneticTest::GetPopulationStr()
{
	std::string PopuStr = "";

	for (auto const& Member : Population)
		PopuStr += "P." + std::to_string(Member.first->GetIndex()) + " ";

	return PopuStr;
}

std::string GeneticTest::GetThresholdsStr(Player* _Target)
{
	std::string ThrStr = "";

	for (unsigned int ThrIndex = 0; ThrIndex < 8; ThrIndex++)
		ThrStr += std::to_string(_Target->GetAI()->GetThresholds()[ThrIndex]) + "  ";

	return ThrStr;
}

void GeneticTest::PrintPopulationFitness()
{
	for (auto const& Member : Population)
		std::cout << "P." << Member.first->GetIndex() << ": " << Member.second << "\n";

	std::cout << "Overall average fitness: " << GetOverallFitness() << "  Target fitness: " << TargetFitness << "\n";
}
