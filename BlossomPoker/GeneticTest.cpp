#include "GeneticTest.h"

#include "Board.h"
#include "Player.h"
#include "HandEvaluator.h"

GeneticTest::GeneticTest()
{
	Evaluator = std::make_shared<HandEvaluator>();

	TestBoard = std::make_shared<Board>(Evaluator, 20, false);
	TestBoard->SetActive(true);

	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());	

	Population.reserve(PopulationSize);

	Writer = std::make_unique<LogWriter>();
}

GeneticTest::~GeneticTest()
{

}

void GeneticTest::Start()
{
	Writer->New("PopS_" + std::to_string(PopulationSize) + " SubjAmt_" + std::to_string(SubjectsAmt) + " GenLimit_" + std::to_string(GenerationLimit));

	InitializePopulation(PopulationSize);

	std::cout << "Settings: \nPopulation Size: " << PopulationSize << "  Subjects Amt: " << SubjectsAmt << " Generation Limit: " << GenerationLimit << "\n\n";
	std::cout << "Population of " << Population.size() << " initialized: " << GetPopulationStr() << "\n";

	Writer->Write(0, "Settings: \n Population Size: " + std::to_string(PopulationSize) + "  Subjects Amt: " + std::to_string(SubjectsAmt) + " Generation Limit: " + std::to_string(GenerationLimit) + "\n\n");
	Writer->Write(0, "Population of " + std::to_string(Population.size()) + " initialized" + GetPopulationStr() + "\n");

	for (auto const& Entry : Population)
		Writer->Write(0, "P." + std::to_string(Entry.TargetPlayer->GetIndex()) + ": (" + GetThresholdsStr(*(Entry.TargetPlayer)) + ")\n");

	Writer->New("PlottingLog");
	Writer->Write(1, "#X Y\n");

	Writer->New("DiversityLog");
	Writer->Write(2, "#X Y\n");
}

void GeneticTest::Update()
{
	std::cout << "\nGeneration " << Generation << ":\n";
	Writer->Write(0, "\nGeneration " + std::to_string(Generation) + ":\n");

	GenerateSubjects(SubjectsAmt);
	
	std::cout << RandomSubjects.size() << " subjects generated...\n";
	Writer->Write(0, std::to_string(RandomSubjects.size()) + " subjects generated...\n");

	MeasureFitness();	
	
	PrintPopulationFitness();
	std::cout << "Diversity of Generation " << Generation << ": " << GetGenerationDiversity() << "\n";
	Writer->Write(2, std::to_string(Generation) + " " + std::to_string(GetGenerationDiversity()) + "\n");

	if (!IsTestComplete())
		ReproducePopulation();
	else
		End();
}

void GeneticTest::End()
{
	std::cout << "Ending Test... (Current: " << GetOverallFitness() << " / Target: " << TargetFitness << ")\n";
	Writer->Write(0, "Ending Test... (Current: " + std::to_string(GetOverallFitness()) + " / Target: " + std::to_string(TargetFitness) + ")\n");

	//std::cout << "Target Fitness Reached ! (Current: " << GetOverallFitness() << " / Target: " << TargetFitness << ")\n";
	//Writer->Write(0, "Target Fitness Reached ! (Current: " + std::to_string(GetOverallFitness()) + " / Target: " + std::to_string(TargetFitness) + ")\n");
	
	Writer->Close(0);
	Writer->Close(1);
	Writer->Close(2);
	Writer->Clear();
}

void GeneticTest::Reset()
{
	Generation = 0;
	PlayersGenerated = 0;

	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	TestBoard->Reset(true);
	Population.clear();
	RandomSubjects.clear();
}

bool GeneticTest::IsTestComplete()
{
	return (GetOverallFitness() >= TargetFitness) || (GenerationLimit > 0 && Generation >= GenerationLimit);
}

void GeneticTest::InitializePopulation(unsigned int _Size) 
{
	Population.clear();

	for (unsigned int Index = 0; Index < _Size; Index++)
	{
		PlayerEntry NewEntry(std::make_shared<Player>(TestBoard, PlayersGenerated), 0.0);
		Population.push_back(NewEntry);
		PlayersGenerated++;
	}
}

void GeneticTest::GenerateSubjects(unsigned int _Size)
{
	RandomSubjects.clear();

	std::uniform_int_distribution<int> Distribution_SubjectIndex(0,1000);

	for (unsigned int Index = 0; Index < _Size; Index++)
		RandomSubjects.push_back(std::make_shared<Player>(TestBoard, Distribution_SubjectIndex(MTGenerator)));
}

double GeneticTest::DetermineWinRate(std::shared_ptr<Player> _Current, std::shared_ptr<Player> _Subject)
{
	TestBoard->Reset(true);

	TestBoard->AddPlayer(_Current);
	TestBoard->AddPlayer(_Subject);

	TestBoard->Start();

	//double OverallProfits = 0;

	//std::cout << "Initial: P." << _Current->GetIndex() << " (S: $" << _Current->GetStack() << "/A: $" << _Current->GetAnte() << ") SP." << _Subject->GetIndex() << " (S: $" << _Subject->GetStack() << "/A: $" << _Subject->GetAnte() << ") Pot ($" << TestBoard->GetPot() << ") \n";

	while (TestBoard->GetRounds() < RoundLimit)
	{
		TestBoard->Update();
		//std::cout << "Round " << TestBoard->GetRounds() << "/" << RoundLimit << " - P." << TestBoard->GetCurrentPlayer()->GetIndex() << "'s turn (" << TestBoard->GetStateStr() << " - $" << TestBoard->GetPot() << ")   \r";

		if (TestBoard->IsGameEnded())
		{
			//OverallProfits += (double) _Current->GetStack() - (double) TestBoard->GetEntryStack();

			TestBoard->Reset(false);

			TestBoard->AddPlayer(_Current);
			TestBoard->AddPlayer(_Subject);

			TestBoard->Start();
		}
	}

	//std::cout << "\n";
	//std::cout << "                                                                                                                        \r";
	//std::cout << "Result: P." << _Current->GetIndex() << " (S: $" << _Current->GetStack() << "/A: $" << _Current->GetAnte() << ") SP." << _Subject->GetIndex() << " (S: $" << _Subject->GetStack() << "/A: $" << _Subject->GetAnte() << ") Pot ($" << TestBoard->GetPot() << ") = Ended at " << TestBoard->GetStateStr() << "\n";
	//std::cout << "P." << _Current->GetIndex() << " against SP." << _Subject->GetIndex() << ": " << (double)TestBoard->GetPlayerWins(_Current) / (double)TestBoard->GetRounds() << " (Wins: " << TestBoard->GetPlayerWins(_Current) << " | Rounds: " << TestBoard->GetRounds() << ")\n";

	//return (((OverallProfits + (double) _Current->GetStack() - (double) TestBoard->GetEntryStack()) / (double) TestBoard->GetBigBlind()) / (double) RoundLimit) * 100.0;

	return (double)TestBoard->GetPlayerWins(_Current) / (double)TestBoard->GetRounds();
}

void GeneticTest::MeasureFitness()
{
	std::shared_ptr<Player> CurrentPlayer = nullptr;
	double AverageWinRate = 0.0;
	unsigned int SubjectInd = 0;

	for (auto & Entry : Population) //auto PopulationItr = Population.begin(); PopulationItr != Population.end(); ++PopulationItr)
	{
		CurrentPlayer = Entry.TargetPlayer;
		AverageWinRate = 0.0;

		for (auto const& Subject : RandomSubjects)
		{
			AverageWinRate += DetermineWinRate(CurrentPlayer, Subject);
			SubjectInd++;

			std::cout << "P." << CurrentPlayer->GetIndex() << " dualing against SP." << Subject->GetIndex() << "... (Subject Done: " << SubjectInd << "/" << RandomSubjects.size() << ")      \r";
		}

		AverageWinRate /= RandomSubjects.size();
		Entry.WinRate = AverageWinRate;

		std::cout << "\n";
		SubjectInd = 0;
	}
}

double GeneticTest::GetOverallFitness()
{
	double TotalFitness = 0.0;
	for (auto const& Entry : Population)
		TotalFitness += Entry.WinRate;

	return TotalFitness / Population.size();
}

double GeneticTest::GetGenerationDiversity()
{
	double AverageSD = 0;
	double SD = 0;
	double Mean = 0;

	for (unsigned int ParamIndex = 0; ParamIndex < 8; ParamIndex++)
	{
		Mean = 0;
		for (auto const& Entry : Population)
			Mean += Entry.TargetPlayer->GetAI().GetThresholds()[ParamIndex];
		Mean /= PopulationSize;

		SD = 0;
		for (auto const& Entry : Population)
			SD += pow((Entry.TargetPlayer->GetAI().GetThresholds()[ParamIndex] - Mean), 2);
		SD /= PopulationSize;
		SD = pow(SD, 0.5);

		AverageSD += SD;
	}

	return AverageSD / 8.0;

	/*double Mean = GetOverallFitness();
	double SD = 0;

	for (auto const& Entry : Population)
		SD += pow((Entry.WinRate - Mean), 2);

	SD /= Population.size();
	SD = pow(SD, 0.5);

	return SD;*/
}

bool GeneticTest::HasHigherFitness(std::pair<std::shared_ptr<Player>, double> _First, std::pair<std::shared_ptr<Player>, double> _Second)
{
	return _First.second >= _Second.second ? true : false;
}

void GeneticTest::SelectParents(const std::vector<PlayerEntry>& _ReferencePop, std::vector<PlayerEntry>& _Parents)
{
	std::uniform_int_distribution<int> Distribution_Qualifier(0, PopulationSize - 1);
	
	std::vector<PlayerEntry> Tourament;
	Tourament.reserve(TouramentSize);

	PlayerEntry CurrentEntry(nullptr, 0.0);

	for (unsigned int Index = 0; Index < ParentLimit; Index++)
	{
		Tourament.clear();

		//Generate tourament
		for (unsigned int TourIndex = 0; TourIndex < TouramentSize; TourIndex++)
		{
			do
			{
				CurrentEntry = _ReferencePop[Distribution_Qualifier(MTGenerator)];
			} 
			while (std::find(Population.begin(),Population.end(), CurrentEntry) != Population.end());

			Tourament.push_back(CurrentEntry);
		}

		//Get fittest in tourament
		CurrentEntry = Tourament[0];

		for (auto const& Entry : Tourament)
		{
			if (Entry.WinRate > CurrentEntry.WinRate)
				CurrentEntry = Entry;
		}

		//Add as valid parent
		_Parents.push_back(CurrentEntry);
	}
}

void GeneticTest::Crossover(const std::shared_ptr<Player>& _First, const std::shared_ptr<Player>& _Second, std::shared_ptr<Player>& _Result)
{
	std::array<double, 8> CombinedThresholds;
	std::uniform_int_distribution<int> Distribution_Crossover(0, 1);

	for (unsigned int ThrIndex = 0; ThrIndex < 8; ThrIndex++)
		CombinedThresholds[ThrIndex] = Distribution_Crossover(MTGenerator) == 0 ? _First->GetAI().GetThresholds()[ThrIndex] : _Second->GetAI().GetThresholds()[ThrIndex];

	_Result = std::move(std::make_shared<Player>(TestBoard, PlayersGenerated, CombinedThresholds));
	PlayersGenerated++;
}

void GeneticTest::Mutate(std::shared_ptr<Player>& _Target, unsigned int _ParaIndex)
{
	//std::uniform_int_distribution<int> Distribution_ThreshIndex(0, 7);
	std::uniform_real_distribution<double> Distribution_Mutation(0.0, 1.0); 
	_Target->GetAI().SetThreshold(_ParaIndex, Distribution_Mutation(MTGenerator));
}

bool GeneticTest::HasMutationHappen()
{
	std::uniform_real_distribution<double> Distribution_MutateChance(0.0, 1.0);
	double MutatingRate = pow((2 + ((7 - 2) / (GenerationLimit - 1)) * (Generation + 1)), -1.0);
	return Distribution_MutateChance(MTGenerator) <= MutatingRate ? true : false;

	/*std::uniform_real_distribution<double> Distribution_MutateChance(0.0, 1.0);
	return Distribution_MutateChance(MTGenerator) <= MutationRate ? true : false;*/
}

void GeneticTest::ReproducePopulation()
{
	std::cout << "Reproducing population...\n";
	Writer->Write(0, "Reproducing population...\n");

	std::vector<PlayerEntry> PopulationReference(Population.begin(), Population.end());
	Population.clear();

	std::vector<PlayerEntry> CurrentParents;
	PlayerEntry ChildEntry(nullptr, 0.0);

	//Generate the same amount of children as the current generation for the next
	for (unsigned int Index = 0; Index < PopulationSize; Index++)
	{
		//Select Parents
		SelectParents(PopulationReference, CurrentParents);
		std::cout << "Parents: (P." << CurrentParents[0].TargetPlayer->GetIndex() << ": " << CurrentParents[0].WinRate << ") & (P." << CurrentParents[1].TargetPlayer->GetIndex() << ": " << CurrentParents[1].WinRate << ")\n";
		//Writer->Write(0, "Parents: (P." + std::to_string(CurrentParents[0].TargetPlayer->GetIndex()) + ": " + std::to_string(CurrentParents[0].WinRate) + ") & (P." + std::to_string(CurrentParents[1].TargetPlayer->GetIndex()) + ": " + std::to_string(CurrentParents[1].WinRate) + ")\n");

		//Cross-over
		Crossover(CurrentParents[0].TargetPlayer, CurrentParents[1].TargetPlayer, ChildEntry.TargetPlayer);
		CurrentParents.clear();

		//Possible Mutation
		for (unsigned int ParaIndex = 0; ParaIndex < 7; ParaIndex++)
		{
			if (HasMutationHappen())
				Mutate(ChildEntry.TargetPlayer,ParaIndex);
		}

		Population.push_back(ChildEntry);
		Writer->Write(0, "Child " + std::to_string(Index) + ": P." + std::to_string(PlayersGenerated) + " (" + GetThresholdsStr(*ChildEntry.TargetPlayer) + ")\n");
	}
	
	Generation++;
}

std::string GeneticTest::GetPopulationStr()
{
	std::string PopuStr = "";

	for (auto const& Entry : Population)
		PopuStr += "P." + std::to_string(Entry.TargetPlayer->GetIndex()) + " ";

	return PopuStr;
}

std::string GeneticTest::GetThresholdsStr(Player& _Target)
{
	std::string ThrStr = "";

	for (unsigned int ThrIndex = 0; ThrIndex < 8; ThrIndex++)
		ThrStr += std::to_string(_Target.GetAI().GetThresholds()[ThrIndex]) + "  ";

	return ThrStr;
}

void GeneticTest::PrintPopulationFitness()
{
	for (auto const& Entry : Population)
		Writer->Write(0, "P." + std::to_string(Entry.TargetPlayer->GetIndex()) + " Average Win Rate: " + std::to_string(Entry.WinRate) + "\n");

	//	std::cout << "P." << Member.first->GetIndex() << ": " << Member.second << "\n";

	std::cout << "Overall average fitness: " << GetOverallFitness() << "  Target fitness: " << TargetFitness << "\n";
	Writer->Write(0, "Overall average fitness: " + std::to_string(GetOverallFitness()) + " Target fitness: " + std::to_string(TargetFitness) + "\n");
	Writer->Write(1, std::to_string(Generation) + " " + std::to_string(GetOverallFitness()) + "\n");
}

void GeneticTest::SetSpecs(unsigned int _PopulationSize, unsigned int _SubjectsSize, unsigned int _GenerationLimit)
{
	PopulationSize = _PopulationSize;
	SubjectsAmt = _SubjectsSize;
	GenerationLimit = _GenerationLimit;
}
