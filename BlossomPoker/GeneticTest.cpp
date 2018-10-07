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

	FittestPlayer = std::make_shared<PlayerEntry>(nullptr,0);

	Writer = std::make_unique<LogWriter>();
}

GeneticTest::~GeneticTest()
{

}

void GeneticTest::Start()
{
	Writer->NewFile(LogType::NONE, "Tourament - PopS_" + std::to_string(PopulationSize) + " SubjAmt_" + std::to_string(SubjectsAmt) + " GenLimit_" + std::to_string(GenerationLimit));

	InitializePopulation(PopulationSize);

	std::cout << "Settings: \nPopulation Size: " << PopulationSize << "  Subjects Amt: " << SubjectsAmt << " Generation Limit: " << GenerationLimit << "\n\n";
	std::cout << "Population of " << Population.size() << " initialized: " << GetPopulationStr() << "\n";

	Writer->WriteAt(0, "Settings: \n Population Size: " + std::to_string(PopulationSize) + "  Subjects Amt: " + std::to_string(SubjectsAmt) + " Generation Limit: " + std::to_string(GenerationLimit) + "\n\n");
	Writer->WriteAt(0, "Population of " + std::to_string(Population.size()) + " initialized" + GetPopulationStr() + "\n");

	for (auto const& Entry : Population)
		Writer->WriteAt(0, "P." + std::to_string(Entry.TargetPlayer->GetIndex()) + ": (" + GetThresholdsStr(*(Entry.TargetPlayer)) + ")\n");

	Writer->NewFile(LogType::Graph_Line, "GenerationPerformance");
	Writer->WriteAt(1, "#X Y\n");

	Writer->NewFile(LogType::Graph_Line, "PopulationVariance");
	Writer->WriteAt(2, "#X Y\n");

	Writer->NewFile(LogType::Graph_Bar, "EvolutionaryProgress");
	Writer->WriteAt(3, "#Best agent in each generation:");
}

void GeneticTest::Update()
{
	std::cout << "\nGeneration " << Generation << ":\n";
	Writer->WriteAt(0, "\nGeneration " + std::to_string(Generation) + ":\n");

	GenerateSubjects(SubjectsAmt);
	
	std::cout << RandomSubjects.size() << " subjects generated...\n";
	Writer->WriteAt(0, std::to_string(RandomSubjects.size()) + " subjects generated...\n");

	MeasureFitness();
	
	PrintPopulationFitness();
	std::cout << "Diversity of Generation " << Generation << ": " << GetGenerationDiversity() << "\n";
	
	Writer->WriteAt(0, "Overall average fitness: " + std::to_string(GetOverallFitness()) + " Best fitness: " + std::to_string(FittestPlayer->WinRate) + " Target fitness: " + std::to_string(TargetFitness) + "\n");
	Writer->WriteAt(1, Generation, GetOverallFitness());
	Writer->WriteAt(2, Generation, GetGenerationDiversity());

	if (!IsTestComplete())
	{
		ReproducePopulation();
	}
	else
		End();
}

void GeneticTest::End()
{
	std::cout << "Ending Test... (Current: " << GetOverallFitness() << " / Target: " << TargetFitness << ")\n";
	Writer->WriteAt(0, "Ending Test... (Current: " + std::to_string(GetOverallFitness()) + " / Target: " + std::to_string(TargetFitness) + ")\n");

	Writer->CloseAt(0);
	Writer->CloseAt(1);
	Writer->CloseAt(2);

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

	while (TestBoard->GetRounds() < RoundLimit)
	{
		TestBoard->Update();

		if (TestBoard->IsGameEnded())
		{
			TestBoard->Reset(false);

			TestBoard->AddPlayer(_Current);
			TestBoard->AddPlayer(_Subject);

			TestBoard->Start();
		}
	}

	return (double)TestBoard->GetPlayerWins(_Current) / (double)TestBoard->GetRounds();
}

void GeneticTest::MeasureFitness()
{
	std::shared_ptr<Player> CurrentPlayer = nullptr;
	double AverageWinRate = 0.0;
	unsigned int SubjectInd = 0;

	FittestPlayer->TargetPlayer = Population[0].TargetPlayer;
	FittestPlayer->WinRate = Population[0].WinRate;

	for (auto & Entry : Population)
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

		if (AverageWinRate > FittestPlayer->WinRate)
		{
			FittestPlayer->TargetPlayer = Entry.TargetPlayer;
			FittestPlayer->WinRate = AverageWinRate;
		}

		std::cout << "\n";
		SubjectInd = 0;
	}

	Writer->WriteAt(3, "#P." + std::to_string(FittestPlayer->TargetPlayer->GetIndex()) + ":" + GetThresholdsStr(*FittestPlayer->TargetPlayer) + "\n");
	//Writer->WriteAt(3,  std::string("# P." + FittestPlayer->TargetPlayer->GetIndex()) + ": (" + GetThresholdsStr(*FittestPlayer->TargetPlayer) + ")");
	FittestInGenerations.push_back(FittestPlayer);
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
}

bool GeneticTest::HasHigherFitness(std::pair<std::shared_ptr<Player>, double> _First, std::pair<std::shared_ptr<Player>, double> _Second)
{
	return _First.second >= _Second.second ? true : false;
}


void GeneticTest::TouramentSelect(const std::vector<PlayerEntry>& _ReferencePop, std::vector<PlayerEntry>& _Parents)
{
	//Tourament Selection
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
	std::uniform_real_distribution<double> Distribution_Mutation(0.0, 1.0); 
	_Target->GetAI().SetThreshold(_ParaIndex, Distribution_Mutation(MTGenerator));
}

bool GeneticTest::HasMutationHappen()
{
	std::uniform_real_distribution<double> Distribution_MutateChance(0.0, 1.0);
	double MutatingRate = pow((2 + ((7 - 2) / (GenerationLimit - 1)) * (Generation + 1)), -1.0);
	return Distribution_MutateChance(MTGenerator) <= MutatingRate ? true : false;
}

void GeneticTest::ReproducePopulation()
{
	std::cout << "Reproducing population...\n";
	Writer->WriteAt(0, "Reproducing population...\n");

	//Alternis Selection
	/*
	std::vector<PlayerEntry> PopulationReference(Population.begin(), Population.end());
	Population.clear();

	std::vector<PlayerEntry> SortedPopulation;
	SortedPopulation.reserve(PopulationSize);

	//std::cout << "Pre-sorted population: ";
	//for (auto const& Entry : PopulationReference)
	//	std::cout << "P." << Entry.TargetPlayer->GetIndex() << "(" << Entry.WinRate << ") ";
	//std::cout << "\n\n";

	//Sort population in descending order of fitness
	std::sort(PopulationReference.begin(), PopulationReference.end(), [](const PlayerEntry& _First, const PlayerEntry& _Second) {return _First.WinRate > _Second.WinRate; });

	//std::cout << "Sort by descending fitness: ";
	//for (auto const& Entry : PopulationReference)
	//	std::cout << "P." << Entry.TargetPlayer->GetIndex() << "(" << Entry.WinRate << ") ";
	//std::cout << "\n\n";

	//Sort population in alternating order of fitness (eg. 1st worst, 1st best, 2nd worst, 2nd best....)
	if (PopulationSize % 2 != 0)
	{
		unsigned int Breakpoint = std::ceil((double)PopulationSize / 2.0);
		
		for (unsigned int Index = 0; Index < Breakpoint; Index++)
		{
			SortedPopulation.push_back(PopulationReference[PopulationSize - 1 - Index]);
			SortedPopulation.push_back(PopulationReference[Index]);
		}

		SortedPopulation.push_back(PopulationReference[Breakpoint - 1]);
	}
	else
	{
		unsigned int Breakpoint = PopulationSize / 2;

		for (unsigned int Index = 0; Index < Breakpoint; Index++)
		{
			SortedPopulation.push_back(PopulationReference[PopulationSize - 1 - Index]);
			SortedPopulation.push_back(PopulationReference[Index]);

		}
	}

	/*std::cout << "Sort by alternating fitness: ";
	for (auto const& Entry : SortedPopulation)
		std::cout << "P." << Entry.TargetPlayer->GetIndex() << "(" << Entry.WinRate << ") ";
	std::cout << "\n\n";

	PopulationReference.clear();
	
	//Select the appropriate elements to mating pool
	unsigned int Target = 0;
	std::uniform_int_distribution<int> Distribution_RandomRange(1, PopulationSize - 2);

	for (unsigned int Index = 0; Index < PopulationSize; Index++)
	{
		if (PopulationReference.size() >= PopulationSize)
			break;

		Target = Distribution_RandomRange(MTGenerator);
		PopulationReference.push_back(SortedPopulation[Target]);

		if (PopulationReference.size() >= PopulationSize)
			break;
	
		PopulationReference.push_back(SortedPopulation[Target - 1]);

		if (PopulationReference.size() >= PopulationSize)
			break;

		PopulationReference.push_back(SortedPopulation[Target + 1]);
	}

	//std::cout << "Mating pool: ";
	//for (auto const& Entry : PopulationReference)
	//	std::cout << "P." << Entry.TargetPlayer->GetIndex() << "(" << Entry.TargetPlayer->GetAI().GetThresholds()[0] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[1] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[2] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[3] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[4] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[5] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[6] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[7] << ") \n";
	//std::cout << "\n\n";

	Distribution_RandomRange.param(std::uniform_int_distribution<>::param_type(0, PopulationSize - 1));
	PlayerEntry ChildEntry(nullptr, 0.0);

	for (unsigned Index = 0; Index < PopulationSize; Index++)
	{
		Crossover(PopulationReference[Distribution_RandomRange(MTGenerator)].TargetPlayer, PopulationReference[Distribution_RandomRange(MTGenerator)].TargetPlayer, ChildEntry.TargetPlayer);

		for (unsigned int ParaIndex = 0; ParaIndex < 7; ParaIndex++)
		{
			if (HasMutationHappen())
				Mutate(ChildEntry.TargetPlayer, ParaIndex);
		}

		Population.push_back(ChildEntry);
		std::cout << "Child " << Index << ": P." << PlayersGenerated << " (" << GetThresholdsStr(*ChildEntry.TargetPlayer) << ")\n";
		Writer->Write(0, "Child " + std::to_string(Index) + ": P." + std::to_string(PlayersGenerated) + " (" + GetThresholdsStr(*ChildEntry.TargetPlayer) + ")\n");
	}

	//std::cout << "Final population: ";
	//for (auto const& Entry : Population)
	//	std::cout << "P." << Entry.TargetPlayer->GetIndex() << "(" << Entry.TargetPlayer->GetAI().GetThresholds()[0] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[1] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[2] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[3] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[4] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[5] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[6] << "," << Entry.TargetPlayer->GetAI().GetThresholds()[7] << ") \n";
	//std::cout << "\n\n";
	*/

	//Tourament Selection
	std::vector<PlayerEntry> PopulationReference(Population.begin(), Population.end());
	Population.clear();

	std::vector<PlayerEntry> CurrentParents;
	PlayerEntry ChildEntry(nullptr, 0.0);

	//Generate the same amount of children as the current generation for the next
	for (unsigned int Index = 0; Index < PopulationSize; Index++)
	{
		//Select Parents
		TouramentSelect(PopulationReference, CurrentParents);
		std::cout << "Parents: (P." << CurrentParents[0].TargetPlayer->GetIndex() << ": " << CurrentParents[0].WinRate << ") & (P." << CurrentParents[1].TargetPlayer->GetIndex() << ": " << CurrentParents[1].WinRate << ")\n";

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
		Writer->WriteAt(0, "Child " + std::to_string(Index) + ": P." + std::to_string(PlayersGenerated) + " (" + GetThresholdsStr(*ChildEntry.TargetPlayer) + ")\n");
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
		Writer->WriteAt(0, "P." + std::to_string(Entry.TargetPlayer->GetIndex()) + " Average Win Rate: " + std::to_string(Entry.WinRate) + "\n");

	std::cout << "Average fitness: " << GetOverallFitness() << " Best Fitness: " << FittestPlayer->WinRate << "  Target fitness: " << TargetFitness << "\n";
}

void GeneticTest::SetSpecs(unsigned int _PopulationSize, unsigned int _SubjectsSize, unsigned int _GenerationLimit)
{
	PopulationSize = _PopulationSize;
	SubjectsAmt = _SubjectsSize;
	GenerationLimit = _GenerationLimit;
}
