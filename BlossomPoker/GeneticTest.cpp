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
	InitializePopulation(5);
	std::cout << "Populaton of " << Population.size() << " initialized: " << GetPopulationStr() << "\n\n";
}

void GeneticTest::Update()
{
	std::cout << "Generation " << Generation << ":\n";

	GenerateSubjects(12);
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
	for (unsigned int TIndex = 0; TIndex < TrialLimit; TIndex++)
	{
		TestBoard->Reset();

		std::cout << "Board Resetted\n";

		TestBoard->AddPlayer(_Current);
		TestBoard->AddPlayer(_Subject);

		TestBoard->Start();

		std::cout << "Initial: P." << _Current->GetIndex() << " (S: $" << _Current->GetStack() << "/A: $" << _Current->GetAnte() << ") SP." << _Subject->GetIndex() << " (S: $" << _Subject->GetStack() << "/A: $" << _Subject->GetAnte() << ") Pot ($" << TestBoard->GetPot() << ") \n";

		for (unsigned int RIndex = 0; RIndex < RoundLimit; RIndex++)
		{
			TestBoard->Update();
	
			if (TestBoard->IsGameEnded())
			{
				std::cout << "Game has ended...\n";
				break;
			}	
		}

		std::cout << "Result: P." << _Current->GetIndex() << " (S: $" << _Current->GetStack() << "/A: $" << _Current->GetAnte() << ") SP." << _Subject->GetIndex() << " (S: $" << _Subject->GetStack() << "/A: $" << _Subject->GetAnte() << ") Pot ($" << TestBoard->GetPot() << ") = Ended at " << TestBoard->GetStateStr() << "\n";
	}

	std::cout << "P." << _Current->GetIndex() << " against SP." << _Subject->GetIndex() << ": " << (double)TestBoard->GetPlayerWins(_Current) / (double)TestBoard->GetRounds() << " (Wins: " << TestBoard->GetPlayerWins(_Current) << " | Rounds: " << TestBoard->GetRounds() << ")\n\n";

	if (TestBoard->GetPlayerWins(_Current) == 0 && TestBoard->GetRounds() == 1)
		std::cout << "ERROR FOUND ... \n";

	return (double)TestBoard->GetPlayerWins(_Current) / (double)TestBoard->GetRounds();
}

void GeneticTest::MeasureFitness()
{
	Player* CurrentPlayer = nullptr;
	double AverageWinRate = 0.0;

	for (auto PopulationItr = Population.begin(); PopulationItr != Population.end(); ++PopulationItr)
	{
		CurrentPlayer = PopulationItr->first;
		AverageWinRate = 0.0;

		for (unsigned int SubjectIndex = 0; SubjectIndex < RandomSubjects.size(); SubjectIndex++)
			AverageWinRate += DetermineWinRate(CurrentPlayer, RandomSubjects[SubjectIndex]);

		AverageWinRate /= RandomSubjects.size();
		PopulationItr->second = AverageWinRate;
	}
}

double GeneticTest::GetOverallFitness()
{
	double TotalFitness = 0.0;
	for (auto Itr = Population.begin(); Itr != Population.end(); ++Itr)
		TotalFitness += Itr->second;

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
	
	//Select Parents from current population via the Touranment algorithm 
	for (unsigned int Index = 0; Index < ParentLimit/WinnerPerTouranment; Index++)
	{
		std::array<std::pair<Player*,double>, 4> Touranment;
		for (unsigned int TourIndex = 0; TourIndex < TouranmentSize; TourIndex++)
			Touranment[TourIndex] = PopulationSample[QualifierDistribution(mt)];

		std::pair<Player*, double> Winner = Touranment[0];
		for (unsigned int PIndex = 1; PIndex < Touranment.size(); PIndex++)
		{
			if (Touranment[PIndex].second > Winner.second)
				Winner = Touranment[PIndex];
		}

		Population.insert(Winner);
		std::cout << "Parent: " << Index << ": P." << Winner.first->GetIndex() << " (" << GetThresholdsStr(Winner.first) << ")\n";
	}

	Player* Parent_A = Population.begin()->first;//nullptr;
	Player* Parent_B = (++Population.begin())->first;//nullptr;
	Player* NewChild = nullptr;

	std::array<double, 8> Thresholds_A = Parent_A->GetAI()->GetThresholds();
	std::array<double, 8> Thresholds_B = Parent_B->GetAI()->GetThresholds();

	std::uniform_int_distribution<int> ParentDistribution(0, ParentLimit - 1);
	std::uniform_int_distribution<int> SelectDistribution(0, 1);
	std::uniform_int_distribution<int> ThresholdDistribution(0, 7);
	std::uniform_real_distribution<double> MutationDistribution(0.0, 1.0);

	//Cross-match the candidates within the population to generate a new set of population
	for (unsigned int Index = 0; Index < PopulationSample.size() - ParentLimit; Index++)
	{
		/*Parent_A = PopulationSample[ParentDistribution(mt)].first;
		Parent_B = PopulationSample[ParentDistribution(mt)].first;

		Thresholds_A = Parent_A->GetAI()->GetThresholds();
		Thresholds_B = Parent_B->GetAI()->GetThresholds();*/
		
		std::array<double, 8> CombinedThresholds;
		for (unsigned int ThrIndex = 0; ThrIndex < 8; ThrIndex++)
			CombinedThresholds[ThrIndex] = SelectDistribution(mt) == 0 ? Thresholds_A[ThrIndex] : Thresholds_B[ThrIndex];

		//Will mutation happen?
		if (HasMutationHappen())
			CombinedThresholds[ThresholdDistribution(mt)] = MutationDistribution(mt);

		NewChild = new Player(TestBoard, PlayersGenerated, CombinedThresholds);
		Population.insert(std::make_pair(NewChild, 0.0));

		std::cout << "Child " << Index << ": P." << PlayersGenerated << " (" << GetThresholdsStr(NewChild) << ")\n";
	}
	
	Generation++;
}

std::string GeneticTest::GetPopulationStr()
{
	std::string PopuStr = "";

	for (auto Itr = Population.begin(); Itr != Population.end(); ++Itr)
		PopuStr += "P." + std::to_string(Itr->first->GetIndex()) + " ";

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
	for (auto Itr = Population.begin(); Itr != Population.end(); ++Itr)
		std::cout << "P." << Itr->first->GetIndex() << ": " << Itr->second << "\n";

	std::cout << "Overall average fitness: " << GetOverallFitness() << "  Target fitness: " << TargetFitness << "\n";
}
