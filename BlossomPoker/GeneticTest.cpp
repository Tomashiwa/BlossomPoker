#include "GeneticTest.h"

#include "HandEvaluator.h"
#include "Table.h"
#include "Participant.h"
#include "Player.h"
#include "BlossomPlayer.h"
#include "Folder.h"
#include "Caller.h"
#include "Raiser.h"
#include "Randomer.h"
#include "Tournament.h"

GeneticTest::GeneticTest()
{
	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	Population.reserve(PopulationSize);

	Evaluator = std::make_shared<HandEvaluator>();
	ActiveTable = std::make_shared<Table>(Evaluator, 20, false);
	Writer = std::make_unique<LogWriter>();

	FoldingPlayer = std::make_unique<Folder>(ActiveTable, 100000);
	CallingPlayer = std::make_unique<Caller>(ActiveTable, 200000);
	RaisingPlayer = std::make_unique<Raiser>(ActiveTable, 300000);
	RandomPlayer = std::make_unique<Randomer>(ActiveTable, 400000);
}

GeneticTest::~GeneticTest()
{

}

void GeneticTest::Start()
{
	GeneratePopulation(PopulationSize);

	PlayingPopulation.clear();
	PlayingPopulation.reserve(TableSize);
	
	PlayingPopulation.push_back(FoldingPlayer);
	PlayingPopulation.push_back(CallingPlayer);
	PlayingPopulation.push_back(RaisingPlayer);
	PlayingPopulation.push_back(RandomPlayer);

	Tournaments.clear();
	Tournaments.reserve(ToursPerGen);
	for (unsigned int Index = 0; Index < ToursPerGen; Index++)
		Tournaments.push_back(std::make_unique<Tournament>(Index, 20, ActiveTable));

	RankingBoard.clear();
	RankingBoard.reserve(PopulationSize);
	for (auto const Player : Population)
		RankingBoard.push_back(std::make_shared<Participant>(Player));

	#pragma region Logging & Comments
	std::cout << "Settings: \nPopulation Size: " << PopulationSize << " Generation Limit: " << GenerationLimit << "\n\n";
	std::cout << "Population of " << Population.size() << " initialized: " << GetPopulationContentStr() << "\n";

	Writer->NewFile(LogType::NONE, "Tournament - PopS_" + std::to_string(PopulationSize) + " GenLimit_" + std::to_string(GenerationLimit) + " ToursPerGen_" + std::to_string(ToursPerGen));
	Writer->WriteAt(0, "Settings: \n Population Size: " + std::to_string(PopulationSize) + "  Subjects Amt: " + " Generation Limit: " + std::to_string(GenerationLimit) + " Tours Per Gen: " + std::to_string(ToursPerGen) + "\n\n");
	Writer->WriteAt(0, "Population of " + std::to_string(Population.size()) + " initialized\n " + GetPopulationContentStr() + "\n");

	for (auto const& Player : Population)
		Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": (" + GetThresholdsStr(Player) + ")\n");

	Writer->NewFile(LogType::Graph_Line, "GenerationPerformance");
	Writer->NewFile(LogType::Graph_Line, "PopulationVariance");
	Writer->NewFile(LogType::Graph_Line, "MutationRate");
	Writer->NewFile(LogType::Graph_Bar, "HallOfFame");

	Writer->WriteAt(1, "#X Y\n");
	Writer->WriteAt(2, "#X Y\n");
	Writer->WriteAt(3, "#Generation | Mutation Rate\n");
	Writer->WriteAt(4, "#Player Index | Fitness\n");
	#pragma endregion
}

void GeneticTest::Run()
{
	Start();

	for (unsigned int GenIndex = 0; GenIndex < GenerationLimit; GenIndex++)
	{
		Writer->WriteAt(0, "\nGeneration " + std::to_string(Generation) + ":\n");
		std::cout << "\nGeneration " << Generation << ":\n";

		if (HoF.size() == 0)
		{
			PlayingPopulation.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, 800001));
			PlayingPopulation.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, 800002));
			PlayingPopulation.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, 800003));
		}
		else
		{
			PlayingPopulation.erase(PlayingPopulation.begin() + 4, PlayingPopulation.end());

			for (unsigned int Index = 0; Index < 3; Index++)
				PlayingPopulation.push_back(HoF[Index]->GetOwner());
		}

		for (auto const& Player : Population)
		{
			std::cout << "P." << Player->GetIndex() << ": \n";
			PlayingPopulation.push_back(Player);

			for (auto const& Tournament : Tournaments)
			{
				std::cout << "\nTournament " << Tournament->GetIndex() << ": \n";

				Tournament->Initialise(PlayingPopulation, PlayingPopulation.size(), true);
				Tournament->Run();
				//Tournament->PrintRankings();
			}

			RankPlayer(Player);
			PlayingPopulation.pop_back();
		}
		
		ArrangePlayers(Population);

		for (unsigned int Index = 0; Index < 3; Index++)
		{
			auto TopItr = std::find(HoF.begin(), HoF.end(), RankingBoard[Index]);

			if (TopItr == HoF.end())
			{
				HoF.push_back(std::make_shared<Participant>(RankingBoard[Index]->GetOwner()));
				HoF[HoF.size() - 1]->SetHandsWon(RankingBoard[Index]->GetHandsWon());
				HoF[HoF.size() - 1]->SetHandsLost(RankingBoard[Index]->GetHandsLost());
				HoF[HoF.size() - 1]->SetMoneyWon(RankingBoard[Index]->GetMoneyWon());
				HoF[HoF.size() - 1]->SetMoneyLost(RankingBoard[Index]->GetMoneyLost());

				HoF[HoF.size() - 1]->UpdateFitness();

				Writer->WriteAt(4, std::to_string(RankingBoard[Index]->GetOwner()->GetIndex()) + " " + std::to_string(RankingBoard[Index]->GetFitness()) + "\n");
			}
			else if (TopItr != HoF.end() && RankingBoard[Index]->GetFitness() > (*TopItr)->GetFitness())
			{
				(*TopItr)->SetHandsWon(RankingBoard[Index]->GetHandsWon());
				(*TopItr)->SetHandsLost(RankingBoard[Index]->GetHandsLost());
				(*TopItr)->SetMoneyWon(RankingBoard[Index]->GetMoneyWon());
				(*TopItr)->SetMoneyLost(RankingBoard[Index]->GetMoneyLost());

				(*TopItr)->UpdateFitness();
			}
		}

		ArrangeHoF();

		std::cout << "\nGenerational Ranking: \n";
		for (auto const& Participant : RankingBoard)
			std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetFitness() << " (MoneyWon: " << Participant->GetMoneyWon() << " HandsWon: " << Participant->GetHandsWon() << " MoneyLost: " << Participant->GetMoneyLost() << " HandsLost: " << Participant->GetHandsLost() << ")\n";
		std::cout << "\n";

		std::cout << "HoF: \n";
		for (auto const& Participant : HoF)
			std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetFitness() << " (MoneyWon: " << Participant->GetMoneyWon() << " HandsWon: " << Participant->GetHandsWon() << " MoneyLost: " << Participant->GetMoneyLost() << " HandsLost: " << Participant->GetHandsLost() << ")\n";
		std::cout << "\n";

		Writer->WriteAt(1, Generation, GetOverallFitness());
		Writer->WriteAt(2, Generation, GetGenerationDiversity());

		std::cout << "Top Players: P." << Population[0]->GetIndex() << " (" << GetParticipant(Population[0]->GetIndex())->GetFitness() << ") " << Population[1]->GetIndex() << " (" << GetParticipant(Population[1]->GetIndex())->GetFitness() << ") " << Population[2]->GetIndex() << " (" << GetParticipant(Population[2]->GetIndex())->GetFitness() << ")\n";
		std::cout << "Overall Fitness of Generation " << Generation << ": " << GetOverallFitness() << "\n";
		std::cout << "Diversity of Generation " << Generation << ": " << GetGenerationDiversity() << "\n";
		
		if (!IsTestComplete())
			ReproducePopulation();
		else
			End();
	}

	/*for (unsigned int GenCount = 0; GenCount < GenerationLimit; GenCount++)
	{
		std::cout << "\nGeneration " << Generation << ":\n";
		Writer->WriteAt(0, "\nGeneration " + std::to_string(Generation) + ":\n");

		std::cout << "Amount of Tournaments: " << Tournaments.size() << "\n";

		for (auto const& Tournament : Tournaments)
		{
			std::cout << "\nTournament " << Tournament->GetIndex() << ": \n";

			Tournament->Initialise(Population, (unsigned int) Population.size(), true);
			Tournament->Run();

			Tournament->PrintRankings();
		}

		RankPlayers();

		HoF.push_back(GetBestPlayer());

		ArrangePlayers(Population);

		std::cout << "\nGenerational Ranking: \n";
		for (auto const& Participant : RankingBoard)
			std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetFitness() << " (MoneyWon: " << Participant->GetMoneyWon() << " HandsWon: " << Participant->GetHandsWon() << " MoneyLost: " << Participant->GetMoneyLost() << " HandsLost: " << Participant->GetHandsLost() << ")\n";
		std::cout << "\n";

		Writer->WriteAt(1, Generation, GetOverallFitness());
		Writer->WriteAt(2, Generation, GetGenerationDiversity());
		
		std::cout << "Best Player: P." << GetBestPlayer()->GetIndex() << " (" << GetParticipant(GetBestPlayer()->GetIndex())->GetFitness() << ")\n";
		std::cout << "Diversity of Generation " << Generation << ": " << GetGenerationDiversity() << "\n";

		if (!IsTestComplete())
			ReproducePopulation();
		else
			End();
	}*/
}

void GeneticTest::End()
{
	std::cout << "Test ended at Generation " << Generation << "\n";
	Writer->WriteAt(0, "Test ended at Generation " + std::to_string(Generation) + "\n");

	Writer->CloseAt(0);
	Writer->CloseAt(1);
	Writer->CloseAt(2);
	Writer->CloseAt(3);

	Writer->Clear();
}

void GeneticTest::Reset()
{
	Generation = 0;
	PlayersGenerated = 0;

	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	Population.clear();
}

bool GeneticTest::IsTestComplete()
{
	return (GenerationLimit > 0 && Generation >= (GenerationLimit - 1));
}

void GeneticTest::GeneratePopulation(unsigned int _Size) 
{
	Population.clear();

	for (unsigned int Index = 0; Index < _Size; Index++)
	{
		Population.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated));
		PlayersGenerated++;
	}
}

void GeneticTest::RankPlayer(const std::shared_ptr<BlossomPlayer>& _Player)
{
	std::shared_ptr<Participant> CurrentParticipant = GetParticipant(_Player->GetIndex());

	for (auto const& Tournament : Tournaments)
	{
		CurrentParticipant->SetMoneyWon(CurrentParticipant->GetMoneyWon() + Tournament->GetParticipant(_Player->GetIndex())->GetMoneyWon());
		CurrentParticipant->SetMoneyLost(CurrentParticipant->GetMoneyLost() + Tournament->GetParticipant(_Player->GetIndex())->GetMoneyLost());
		
		CurrentParticipant->SetHandsWon(CurrentParticipant->GetHandsWon() + Tournament->GetParticipant(_Player->GetIndex())->GetHandsWon());
		CurrentParticipant->SetHandsLost(CurrentParticipant->GetHandsLost() + Tournament->GetParticipant(_Player->GetIndex())->GetHandsLost());
	}

	for (auto const& Participant : RankingBoard)
		Participant->UpdateFitness();

	std::sort(RankingBoard.begin(), RankingBoard.end(),
		[](const std::shared_ptr<Participant>& _First, const std::shared_ptr<Participant>& _Second)
		{return _First->GetFitness() > _Second->GetFitness(); }); 
	

	/*for (auto const& Tournament : Tournaments)
	{
		for (auto const& Participant : RankingBoard)
		{
			Participant->SetMoneyWon(Participant->GetMoneyWon() + Tournament->GetParticipant(Participant->GetOwner()->GetIndex())->GetMoneyWon());
			Participant->SetMoneyLost(Participant->GetMoneyLost() + Tournament->GetParticipant(Participant->GetOwner()->GetIndex())->GetMoneyLost());

			Participant->SetHandsWon(Participant->GetHandsWon() + Tournament->GetParticipant(Participant->GetOwner()->GetIndex())->GetHandsWon());
			Participant->SetHandsLost(Participant->GetHandsLost() + Tournament->GetParticipant(Participant->GetOwner()->GetIndex())->GetHandsLost());
		}
	}

	//Average out Players' Rankings
	for (auto const& Participant : RankingBoard)
		Participant->UpdateFitness();

	//Sort the RankingBoard's participant from highest profit to lowest profit
	std::sort(RankingBoard.begin(), RankingBoard.end(),
		[](const std::shared_ptr<Participant>& _First, const std::shared_ptr<Participant>& _Second)
		{return _First->GetFitness() > _Second->GetFitness(); });*/
}

void GeneticTest::ArrangePlayers(std::vector<std::shared_ptr<BlossomPlayer>>& _Players)
{
	std::sort(_Players.begin(), _Players.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) { return GetParticipant(_First->GetIndex())->GetFitness() > GetParticipant(_Second->GetIndex())->GetFitness(); });

	/*_Players.clear();
	
	auto Lamb_GetNextPlayer = [&]() {
		for (auto const& Participant : RankingBoard)
		{
			bool IsValid = std::find_if(_Players.begin(), _Players.end(), [&](std::shared_ptr<BlossomPlayer> _Player) { return Participant->GetOwner()->GetIndex() == _Player->GetIndex(); }) == _Players.end();

			if (IsValid)
				return Participant->GetOwner();
		}
	};

	std::generate_n(std::back_inserter(_Players), RankingBoard.size(), Lamb_GetNextPlayer);*/
}

void GeneticTest::ArrangeHoF()
{
	std::sort(HoF.begin(), HoF.end(),
		[&](std::shared_ptr<Participant>& _First, std::shared_ptr<Participant>& _Second) { return _First->GetFitness() > _Second->GetFitness(); });
}

const std::shared_ptr<Participant>& GeneticTest::GetParticipant(unsigned int _Index)
{
	for (auto const& Participant : RankingBoard)
	{
		if (Participant->GetOwner()->GetIndex() == _Index)
			return Participant;
	}

	return RankingBoard[0];
}

const std::shared_ptr<Player>& GeneticTest::GetBestPlayer()
{
	return RankingBoard[0]->GetOwner();
}

float GeneticTest::GetOverallFitness()
{
	float TotalFitness = 0.0;
	for (auto const& Participant : RankingBoard)
		TotalFitness += Participant->GetFitness();

	return TotalFitness / RankingBoard.size();

	/*float TotalRanking = 0.0;
	for (auto const& Participant : RankingBoard)
		TotalRanking += Participant->Get_Rank();

	std::cout << "Average Ranking: " << (TotalRanking / RankingBoard.size()) << " (Total: " << TotalRanking << " / Table Size: " << RankingBoard.size() << ")\n" ;
	return TotalRanking / RankingBoard.size();*/
}

float GeneticTest::GetGenerationDiversity()
{
	float AverageSD = 0;
	float SD = 0;
	float Mean = 0;

	for (unsigned int ThrIndex = 0; ThrIndex < 16; ThrIndex++)
	{
		Mean = 0;
		for (auto const Player : Population)
			Mean += Player->GetAI().GetThresholds()[ThrIndex];
		Mean /= PopulationSize;

		SD = 0;
		for (auto const Player : Population)
			SD += pow((Player->GetAI().GetThresholds()[ThrIndex] - Mean), 2);
		SD /= PopulationSize;
		SD = pow(SD, (float) 0.5);

		AverageSD += SD;
	}

	return AverageSD / PopulationSize;
}

void GeneticTest::TouramentSelect(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation, std::vector<std::shared_ptr<BlossomPlayer>>& _Parents)
{
	std::uniform_int_distribution<int> Distribution_Qualifier(0, PopulationSize - 1);

	std::vector<std::shared_ptr<BlossomPlayer>> Tournament;
	Tournament.reserve(TouramentSize);

	std::shared_ptr<BlossomPlayer> CurrentPlayer;

	for (unsigned int Index = 0; Index < ParentLimit; Index++)
	{
		Tournament.clear();

		for (unsigned int TourIndex = 0; TourIndex < TouramentSize; TourIndex++)
		{
			do { CurrentPlayer = _RefPopulation[Distribution_Qualifier(MTGenerator)]; } 
				while (std::find(Population.begin(), Population.end(), CurrentPlayer) != Population.end());

			Tournament.push_back(CurrentPlayer);
		}

		CurrentPlayer = Tournament[0];
		
		for (auto const Player : Tournament)
		{
			if (GetParticipant(Player->GetIndex())->GetFitness() > GetParticipant(CurrentPlayer->GetIndex())->GetFitness())//GetParticipant(Player->GetIndex())->Get_Rank() > GetParticipant(CurrentPlayer->GetIndex())->Get_Rank())
				CurrentPlayer = Player;
		}

		if (std::find(_Parents.begin(), _Parents.end(), CurrentPlayer) == _Parents.end())
			_Parents.push_back(CurrentPlayer);
		else
			Index--;
	}
}

void GeneticTest::Crossover(const std::shared_ptr<BlossomPlayer>& _First, const std::shared_ptr<BlossomPlayer>& _Second, std::shared_ptr<BlossomPlayer>& _Result)
{
	std::array<float, 16> CombinedThresholds;
	std::uniform_int_distribution<int> Distribution_Crossover(0, 1);

	for (unsigned int ThrIndex = 0; ThrIndex < 16; ThrIndex++)
		CombinedThresholds[ThrIndex] = Distribution_Crossover(MTGenerator) == 0 ? _First->GetAI().GetThresholds()[ThrIndex] : _Second->GetAI().GetThresholds()[ThrIndex];

	_Result = std::move(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated));
	_Result->GetAI().SetThresholds(CombinedThresholds);
	_Result->GetAI().SetEvalutor(Evaluator);

	PlayersGenerated++;
}	

void GeneticTest::Mutate(std::shared_ptr<BlossomPlayer>& _Target, Phase _Phase, unsigned int _ParaIndex)
{
	std::uniform_real_distribution<float> Distribution_Mutation(-MutateAmt, MutateAmt);

	float MutatedThreshold = _Target->GetAI().GetThresholdsByPhase(_Phase)[_ParaIndex] + Distribution_Mutation(MTGenerator);
	
	if (MutatedThreshold < 0)
	{
		std::uniform_real_distribution<float> Distribution_Backtrack(0.0, 0.25);
		MutatedThreshold = Distribution_Backtrack(MTGenerator);
	}

	_Target->GetAI().SetThresholdByPhase(_Phase, _ParaIndex, MutatedThreshold);
}

bool GeneticTest::HasMutationHappen()
{
	std::uniform_real_distribution<float> Distribution_MutateChance(0.0, 1.0);

	//Gaussian Distribution
	/*float a = 2.5f, b = 0.5f, c = 0.15f;
	float x = (float) Generation / (float) GenerationLimit;
	float e = std::exp(1.0f);
	float MutatingRate = pow((a * e), -(pow((x - b), 2) / (2 * pow(c, 2))));*/

	//Oscillating Sine Wave
	float a = 48.7f;
	float x = (float)Generation / (float)GenerationLimit;
	float MutatingRate = (sin(a * sqrt(x))) / 2.0f + 0.5f;

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
		unsigned int Breakpoint = std::ceil((float)PopulationSize / 2.0);
		
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

	//Tournament Selection
	std::vector<std::shared_ptr<BlossomPlayer>> PopulationReference(Population.begin(), Population.end());
	Population.clear();

	std::vector<std::shared_ptr<BlossomPlayer>> CurrentParents;
	std::shared_ptr<BlossomPlayer> CurrentChild;

	//Generate the same amount of children as the current generation for the next
	for (unsigned int Index = 0; Index < PopulationSize; Index++)
	{
		//Select Parents
		TouramentSelect(PopulationReference, CurrentParents);

		//Cross-over
		Crossover(CurrentParents[0], CurrentParents[1], CurrentChild);

		//Gaussian Mutation
		/*float a = 2.5f, b = 0.5f, c = 0.15f;
		float x = (float)Generation / (float)GenerationLimit;
		float e = std::exp(1.0f);
		float MutatingRate = pow((a * e), -(pow((x - b), 2) / (2 * pow(c, 2))));*/

		//Oscillating Sine Wave
		float a = 48.7f;
		float x = (float)Generation / (float)GenerationLimit;
		float MutatingRate = (sin(a * sqrt(x))) / 2.0f + 0.5f;

		Writer->WriteAt(3, (float) Generation, MutatingRate);

		//Possible Mutation
		for (unsigned int PhaseIndex = 0; PhaseIndex < 4; PhaseIndex++)
		{
			for (unsigned int ParaIndex = 0; ParaIndex < 4; ParaIndex++)
			{
				if (HasMutationHappen())
					Mutate(CurrentChild, (Phase)PhaseIndex, ParaIndex);
			}
		}

		Population.push_back(CurrentChild);

		Writer->WriteAt(0, "Parents: P." + std::to_string(CurrentParents[0]->GetIndex()) + " & P." + std::to_string(CurrentParents[1]->GetIndex()) + " \n");
		Writer->WriteAt(0, "Child " + std::to_string(Index) + ": P." + std::to_string(PlayersGenerated) + " (" + GetThresholdsStr(CurrentChild) + ")\n");
		CurrentParents.clear();
	}

	std::cout << "Population after reproduction: \n";
	for (unsigned int Index = 0; Index < Population.size(); Index++)
	{
		RankingBoard[Index]->Refresh();
		RankingBoard[Index]->SetOwner(Population[Index]);
	
		std::cout << "P." << Population[Index]->GetIndex() << "\n";
	}
	
	Generation++;
}

std::string GeneticTest::GetPopulationContentStr()
{
	std::string PopuStr = "";

	for (auto const& Player : Population)
		PopuStr += "P." + std::to_string(Player->GetIndex()) + " ";

	return PopuStr;
}

std::string GeneticTest::GetThresholdsStr(const std::shared_ptr<BlossomPlayer>& _Target)
{
	std::string ThrStr = "";

	for (unsigned int ThrIndex = 0; ThrIndex < 16; ThrIndex++)
		ThrStr += std::to_string(_Target->GetAI().GetThresholds()[ThrIndex]) + "  ";

	return ThrStr;
}

void GeneticTest::SetSpecs(unsigned int _PopulationSize, unsigned int _GenerationLimit, unsigned int _ToursPerGen)
{
	PopulationSize = _PopulationSize;
	GenerationLimit = _GenerationLimit;
	ToursPerGen = _ToursPerGen;
}
