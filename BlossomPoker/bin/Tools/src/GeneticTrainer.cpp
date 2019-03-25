#include "../inc/GeneticTrainer.h"

#include "../../Cards/inc/HandEvaluator.h"
#include "../../Table/inc/Table.h"
#include "../../Table/inc/Participant.h"
#include "../../Player/inc/Player.h"
#include "../../AI/inc/BlossomPlayer.h"
#include "../../Player/inc/Folder.h"
#include "../../Player/inc/Caller.h"
#include "../../Player/inc/Raiser.h"
#include "../../Player/inc/Randomer.h"
#include "../../Table/inc/Tournament.h"

#include "../../Tools/inc/Precomputation.h"

GeneticTrainer::GeneticTrainer()
{
	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	Evaluator = std::make_shared<HandEvaluator>();
	ActiveTable = std::make_shared<Table>(Evaluator, 20, false);
	Writer = std::make_unique<LogWriter>();

	CallingPlayer = std::make_unique<Caller>(ActiveTable, 200000);
	RaisingPlayer = std::make_unique<Raiser>(ActiveTable, 300000);
	RandomPlayer0 = std::make_unique<Randomer>(ActiveTable, 400000);
	RandomPlayer1 = std::make_unique<Randomer>(ActiveTable, 100000);

	Precomp = std::make_unique<Precomputation>(Evaluator);
}

GeneticTrainer::~GeneticTrainer()
{

}

void GeneticTrainer::Start()
{
	/*std::array<Card, 2> Hole0;
	Hole0[0] = Card(Suit::Diamond, Rank::Two);
	Hole0[1] = Card(Suit::Heart, Rank::Two);
	
	std::vector<Card> Comm0;
	Comm0.push_back(Card(Suit::Club, Rank::Three));
	Comm0.push_back(Card(Suit::Spade, Rank::Three));
	Comm0.push_back(Card(Suit::Club, Rank::Four));

	std::size_t Hash = 0;

	for (auto const& Card : Hole0)
	{
		unsigned int HashedCard = static_cast<int>(Card);
		HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
		HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
		HashedCard = (HashedCard >> 16) ^ HashedCard;

		Hash += HashedCard;
	}

	for (auto const& Card : Comm0)
	{
		unsigned int HashedCard = static_cast<int>(Card);
		HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
		HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
		HashedCard = (HashedCard >> 16) ^ HashedCard;

		Hash += HashedCard;
	}

	std::cout << Hole0[0].To_String() << Hole0[1].To_String() << " " << Comm0[0].To_String() << Comm0[1].To_String() << Comm0[2].To_String() << " " << Hash << "\n";

	std::array<Card, 2> Hole1;
	Hole1[0] = Card(Suit::Diamond, Rank::Two);
	Hole1[1] = Card(Suit::Heart, Rank::Two);

	std::vector<Card> Comm1;
	Comm1.push_back(Card(Suit::Club, Rank::Three));
	Comm1.push_back(Card(Suit::Diamond, Rank::Four));
	Comm1.push_back(Card(Suit::Diamond, Rank::Three));

	Hash = 0;

	for (auto const& Card : Hole1)
	{
		unsigned int HashedCard = static_cast<int>(Card);
		HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
		HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
		HashedCard = (HashedCard >> 16) ^ HashedCard;

		Hash += HashedCard;
	}

	for (auto const& Card : Comm1)
	{
		unsigned int HashedCard = static_cast<int>(Card);
		HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
		HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
		HashedCard = (HashedCard >> 16) ^ HashedCard;

		Hash += HashedCard;
	}

	std::cout << Hole1[0].To_String() << Hole1[1].To_String() << " " << Comm1[0].To_String() << Comm1[1].To_String() << Comm1[2].To_String() << " " << Hash << "\n";

	std::array<Card, 2> Hole2;
	Hole2[0] = Card(Suit::Heart, Rank::Two);
	Hole2[1] = Card(Suit::Diamond, Rank::Two);

	std::vector<Card> Comm2;
	Comm2.push_back(Card(Suit::Diamond, Rank::Four));
	Comm2.push_back(Card(Suit::Diamond, Rank::Three));
	Comm2.push_back(Card(Suit::Club, Rank::Three));

	Hash = 0;

	for (auto const& Card : Hole2)
	{
		unsigned int HashedCard = static_cast<int>(Card);
		HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
		HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
		HashedCard = (HashedCard >> 16) ^ HashedCard;

		Hash += HashedCard;
	}

	for (auto const& Card : Comm2)
	{
		unsigned int HashedCard = static_cast<int>(Card);
		HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
		HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
		HashedCard = (HashedCard >> 16) ^ HashedCard;

		Hash += HashedCard;
	}

	std::cout << Hole2[0].To_String() << Hole2[1].To_String() << " " << Comm2[0].To_String() << Comm2[1].To_String() << Comm2[2].To_String() << " " << Hash << "\n";
	std::cout << "\n";*/

	/*std::cout << "Odds for " << Hole[0].To_String() << Hole[1].To_String() << " " << Comm[0].To_String() << Comm[1].To_String() << Comm[2].To_String() << ":\n";
	for (unsigned int Count = 1; Count <= 8; Count++)
		std::cout << "Against " << Count << " opponents - " << Evaluator->DetermineOdds_Flop(Hole, Comm, Count) << "\n";
	std::cout << "\n";*/

	//Precomp->ComputePreflopOdds(7, 2500);
	/*Precomp->ComputeFlopOdds(7, 2500);
	std::cout << "\n";*/

	InitializePopu(PopulationSize);

	PlayingPopulation.clear();
	PlayingPopulation.reserve(TableSize);
	
	PlayingPopulation.push_back(RandomPlayer1);
	PlayingPopulation.push_back(CallingPlayer);
	PlayingPopulation.push_back(RaisingPlayer);
	PlayingPopulation.push_back(RandomPlayer0);

	Tournaments.clear();
	Tournaments.reserve(ToursPerGen);
	for (unsigned int Index = 0; Index < ToursPerGen; Index++)
		Tournaments.push_back(std::make_unique<Tournament>(Index, 20, ActiveTable));

	RankingBoard.clear();
	RankingBoard.reserve(PopulationSize);
	for (auto const Player : Population)
		RankingBoard.push_back(std::make_shared<Participant>(Player));

	HoF.clear();

	#pragma region Logging & Comments
	std::cout << "Simulating a Population of " << PopulationSize << " Players for " << GenerationLimit << " generations.\n";
	std::cout << "Players will play " << ToursPerGen << " Tournaments with " << TableSize << " duplicate matches.\n";
	std::cout << "Each generation will crossover at a probability of " << CrossoverRate << " with Elitism (" << ElitesLimit << ") applied and mutate dynamically with delta of " << MutateDelta << ".\n";

	Writer->NewDir();
	Writer->GenerateGNUFiles();

	Writer->NewFile(LogType::NONE, "Tournament - PopS_" + std::to_string(PopulationSize) + " GenLimit_" + std::to_string(GenerationLimit) + " ToursPerGen_" + std::to_string(ToursPerGen));
	Writer->WriteAt(0, "Simulating a Population of " + std::to_string(PopulationSize) + " Players for " + std::to_string(GenerationLimit) + " generations.\n");
	Writer->WriteAt(0, "Players will play " + std::to_string(ToursPerGen) + " Tournaments with " + std::to_string(PopulationSize) + " duplicate matches.\n");
	Writer->WriteAt(0, "Each generation will crossover at a probability of " + std::to_string(CrossoverRate) + " with Elitism (" + std::to_string(ElitesLimit) + ") applied and mutate dynamically with delta of " + std::to_string(MutateDelta) + ".\n");

	Writer->NewFile(LogType::Graph_Line, "GenerationPerformance");
	Writer->NewFile(LogType::Graph_Line, "PopulationVariance");
	Writer->NewFile(LogType::Graph_Line, "MutationRate");
	Writer->NewFile(LogType::Graph_Bar, "HallOfFame");
	Writer->NewFile(LogType::Graph_Line, "GenBestFitness");

	Writer->NewFile(LogType::Graph_Line, "GenWorstFitness");
	Writer->NewFile(LogType::Graph_Line, "GenEliteAverFitness");
	Writer->NewFile(LogType::Graph_Line, "GenHoFTop3AverFitness");

	Writer->WriteAt(1, "#Generation | Average Fitness\n");
	Writer->WriteAt(2, "#Generation | Variance\n");
	Writer->WriteAt(3, "#Generation | Mutation Rate\n");
	Writer->WriteAt(4, "#Player Index | Fitness\n");
	Writer->WriteAt(5, "#Generation | Best Fitness\n");

	Writer->WriteAt(6, "#Generation | Worst Fitness\n");
	Writer->WriteAt(7, "#Generation | Average Fitness\n");
	Writer->WriteAt(8, "#Generation | Top 3 Average Fitness\n");
	#pragma endregion
}

void GeneticTrainer::Run()
{
	Start();

	for (unsigned int GenIndex = 0; GenIndex < GenerationLimit; GenIndex++)
	{
		#pragma region Couts & Log Writing
		Writer->WriteAt(0, "\nGeneration " + std::to_string(Generation) + ": (" + std::to_string(PlayersGenerated) + " Players created till now): \n");
		std::cout << "\nGeneration " << Generation << ": (" << PlayersGenerated << " Players created till now): \n";
		for (auto const& Player : Population)
		{
			Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + GetThresholdsStr(Player) + "\n");
			//std::cout << "P." << Player->GetIndex() << ": (" << GetThresholdsStr(Player) << ")\n";
		}
		//std::cout << "\n";
		Writer->WriteAt(0, "\n");
		#pragma endregion

		//Initialize/Refresh PlayingPopulation
		InitializePlayingPopu();

		//Run tournaments for each player and rank them based on their performance
		for (auto const& Player : Population)
		{
			//std::cout << "\nP." << Player->GetIndex() << ": \n";
			MeasureFitness(Player);
		}
		
		ArrangePlayers(Population);

		//Measure Uniqueness & Potential of each player
		for (auto const& Player : Population)
		{
			MeasureUniqueness(Player);
			MeasurePotential(Player);
		}

		//Add the top players into HoF if they perform sufficently well
		AddPlayersToHoF(PopulationSize);
		ArrangeHoF();
		ClipHoF(PopulationSize / 2);

		#pragma region Couts & Log Writing	
		//std::cout << "\nGenerational Ranking: \n";
		Writer->WriteAt(0, "\nGenerational Ranking: \n");
		for (auto const& Participant : RankingBoard)
		{
			//std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetFitness() << " (Hands W/L: " << Participant->GetHandsWon() << "/" << Participant->GetHandsLost() << " Money W/L: " << Participant->GetMoneyWon() << "/" << Participant->GetMoneyLost() << ")\n";
			Writer->WriteAt(0, "P." + std::to_string(Participant->GetOwner()->GetIndex()) + ": " + std::to_string(Participant->GetFitness()) + "(Hands W/L: " + std::to_string(Participant->GetHandsWon()) + "/" + std::to_string(Participant->GetHandsLost()) + " Money W/L : " + std::to_string(Participant->GetMoneyWon()) + "/" + std::to_string(Participant->GetMoneyLost()) + ")\n");
		}
		//std::cout << "\n";
		Writer->WriteAt(0, "\n");

		//std::cout << "Hall of Fame: \n";
		Writer->WriteAt(0, "Hall of Fame: \n");
		for (auto const& Participant : HoF)
		{
			//std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetFitness() << " (Hands W/L: " << Participant->GetHandsWon() << "/" << Participant->GetHandsLost() << " Money W/L: " << Participant->GetMoneyWon() << "/" << Participant->GetMoneyLost() << ")\n";
			Writer->WriteAt(0, "P." + std::to_string(Participant->GetOwner()->GetIndex()) + ": " + std::to_string(Participant->GetFitness()) + "(Hands W/L: " + std::to_string(Participant->GetHandsWon()) + "/" + std::to_string(Participant->GetHandsLost()) + " Money W/L : " + std::to_string(Participant->GetMoneyWon()) + "/" + std::to_string(Participant->GetMoneyLost()) + ")\n");
		}
		//std::cout << "\n";
		Writer->WriteAt(0, "\n");

		Writer->WriteAt(1, Generation, GetOverallFitness());
		Writer->WriteAt(2, Generation, GetGenerationDiversity());

		std::cout << "Top Players: P." << Population[0]->GetIndex() << " (" << GetParticipant(Population[0]->GetIndex())->GetFitness() << ") P." << Population[1]->GetIndex() << " (" << GetParticipant(Population[1]->GetIndex())->GetFitness() << ") " << Population[2]->GetIndex() << " (" << GetParticipant(Population[2]->GetIndex())->GetFitness() << ")\n";
		Writer->WriteAt(0, "Top Players: P." + std::to_string(Population[0]->GetIndex()) + " (" + std::to_string(GetParticipant(Population[0]->GetIndex())->GetFitness()) + ") P." + std::to_string(Population[1]->GetIndex()) + " (" + std::to_string(GetParticipant(Population[1]->GetIndex())->GetFitness()) + ") P." + std::to_string(Population[2]->GetIndex()) + " (" + std::to_string(GetParticipant(Population[2]->GetIndex())->GetFitness()) + ")\n");
		std::cout << "Overall Fitness of Generation " << Generation << ": " << GetOverallFitness() << "\n";
		Writer->WriteAt(0, "Overall Fitness of Generation " + std::to_string(Generation) + ": " + std::to_string(GetOverallFitness()) + "\n");
		std::cout << "Best Fitness of Generation " << Generation << ": P." << RankingBoard[0]->GetOwner()->GetIndex() << " - " << RankingBoard[0]->GetFitness() << "\n";
		Writer->WriteAt(5, Generation, RankingBoard[0]->GetFitness());
		std::cout << "Worst Fitness of Generation " << Generation << ": P." << RankingBoard[RankingBoard.size() - 1]->GetOwner()->GetIndex() << " - " << RankingBoard[RankingBoard.size() - 1]->GetFitness() << "\n";
		Writer->WriteAt(6, Generation, RankingBoard[RankingBoard.size() - 1]->GetFitness());
		std::wcout << "Average Fitness of Top 3 in HoF:" << (HoF[0]->GetFitness() + HoF[1]->GetFitness() + HoF[2]->GetFitness()) / 3.0f << "\n";
		Writer->WriteAt(8, Generation, (HoF[0]->GetFitness() + HoF[1]->GetFitness() + HoF[2]->GetFitness()) / 3.0f);
		std::cout << "Diversity of Generation " << Generation << ": " << GetGenerationDiversity() << "\n";
		Writer->WriteAt(0, "Diversity of Generation " + std::to_string(Generation) + ": " + std::to_string(GetGenerationDiversity()) + "\n");

		std::cout << "Average Divergence of Thresholds: ";
		Writer->WriteAt(0, "Average Divergence of Thresholds: ");
		for (unsigned int Index = 0; Index < 16; Index++)
		{
			float Average = 0.0f;
			for (auto const& Player : Population)
				Average += Player->GetAI().GetThresholds()[Index];
			Average /= Population.size();

			float StdDev = 0.0f;
			for (auto const& Player : Population)
				StdDev += powf((Player->GetAI().GetThresholds()[Index] - Average), 2.0f);
			StdDev /= Population.size();
			StdDev = powf(StdDev, 0.5f);

			std::cout << StdDev << " ";
			Writer->WriteAt(0, std::to_string(StdDev) + " ");
		}
		std::cout << "\n";
		Writer->WriteAt(0, "\n");
		#pragma endregion

		GenerationAverFitness.push_back(GetOverallFitness());

		if (!IsTestComplete() && HasPopulationStagnate())
			CullCount < MaxCullCount ? CullPopulation() : NukePopulation();
		else if(!IsTestComplete())
			ReproducePopulation();
		else
			End();
	}
}

void GeneticTrainer::End()
{
	std::cout << "Test ended at Generation " << Generation << "\n";
	Writer->WriteAt(0, "Test ended at Generation " + std::to_string(Generation) + "\n\n");

	Writer->CloseAt(0);
	Writer->CloseAt(1);
	Writer->CloseAt(2);
	Writer->CloseAt(3);

	Writer->Clear();
}

void GeneticTrainer::Reset()
{
	Generation = 0;
	PlayersGenerated = 0;

	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	Population.clear();
}

bool GeneticTrainer::IsTestComplete()
{
	return (GenerationLimit > 0 && Generation >= (GenerationLimit - 1));
}

void GeneticTrainer::InitializePopu(unsigned int _Size) 
{
	Population.clear();

	for (unsigned int Index = 0; Index < _Size; Index++)
	{
		Population.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated));
		PlayersGenerated++;
	}
}

void GeneticTrainer::InitializePlayingPopu()
{
	if (HoF.size() == 0)
	{
		PlayingPopulation.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, 800001));
		PlayingPopulation.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, 800002));
		PlayingPopulation.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, 800003));
	}
	else
	{
		PlayingPopulation.erase(PlayingPopulation.begin() + 4, PlayingPopulation.end());

		for (auto const& Participant : HoF)
		{
			if (std::find_if(PlayingPopulation.begin(), PlayingPopulation.end(), [&](std::shared_ptr<Player> _Player) { return _Player->GetIndex() == Participant->GetOwner()->GetIndex(); }) == PlayingPopulation.end() &&
				std::find_if(Population.begin(), Population.end(), [&](std::shared_ptr<BlossomPlayer> _Player) { return _Player->GetIndex() == Participant->GetOwner()->GetIndex(); }) == Population.end())
			{
				PlayingPopulation.push_back(Participant->GetOwner());
			}

			if (PlayingPopulation.size() >= TableSize - 1)
				break;
		}

		if (PlayingPopulation.size() < TableSize - 1)
		{
			std::cout << "Insufficient players from HoF, supplementing with random BlossomPlayer...\n";
			unsigned int ToBeAdded = (TableSize - 1) - PlayingPopulation.size();
			
			for (unsigned int Index = 0; Index < ToBeAdded; Index++)
			{
				PlayingPopulation.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, 800001 + Index));
				std::cout << "Added supplement player P." << PlayingPopulation[PlayingPopulation.size() - 1]->GetIndex() << "\n";
			}
		}
		
		std::cout << "\n";
	}
}

float GeneticTrainer::MeasureFitness(const std::shared_ptr<BlossomPlayer>& _Player)
{
	/*if (PlayingPopulation.size() >= TableSize)
	{
		PlayingPopulation.erase(PlayingPopulation.begin() + TableSize - 1, PlayingPopulation.end());
	}*/
	if(PlayingPopulation.size() < TableSize - 1)
	{
		//std::cout << "PlayingPopulation is not properly initialized...\n";
		return 0.0f;
	}

	PlayingPopulation.push_back(_Player);

	if (std::find_if(RankingBoard.begin(), RankingBoard.end(), [&](const std::shared_ptr<Participant>& _Parti) {return _Parti->GetOwner()->GetIndex() == _Player->GetIndex(); }) != RankingBoard.end())
	{
		for (auto const& Tournament : Tournaments)
		{
			//std::cout << "\nTournament " << Tournament->GetIndex() << ": \n";

			Tournament->Initialise(PlayingPopulation, PlayingPopulation.size(), true);
			Tournament->Run();
		}

		RankPlayer(_Player);
		PlayingPopulation.pop_back();
		
		if (GetParticipant(_Player->GetIndex())->GetHandsWon() == 0 && GetParticipant(_Player->GetIndex())->GetHandsLost() == 0)
			std::cout << "\n";

		return GetParticipant(_Player->GetIndex())->GetFitness();
	}
		
	std::shared_ptr<Participant> CurrentParticipant = std::make_shared<Participant>(_Player);

	for (auto const& Tournament : Tournaments)
	{
		//std::cout << "\nMutated Tournament " << Tournament->GetIndex() << ": \n";

		Tournament->Initialise(PlayingPopulation, PlayingPopulation.size(), true);
		Tournament->Run();

		CurrentParticipant->SetHandsWon(CurrentParticipant->GetHandsWon() + Tournament->GetParticipant(_Player->GetIndex())->GetHandsWon());
		CurrentParticipant->SetHandsLost(CurrentParticipant->GetHandsLost() + Tournament->GetParticipant(_Player->GetIndex())->GetHandsLost());
		CurrentParticipant->SetMoneyWon(CurrentParticipant->GetMoneyWon() + Tournament->GetParticipant(_Player->GetIndex())->GetMoneyWon());
		CurrentParticipant->SetMoneyLost(CurrentParticipant->GetMoneyLost() + Tournament->GetParticipant(_Player->GetIndex())->GetMoneyLost());
	}

	CurrentParticipant->UpdateFitness();
	PlayingPopulation.pop_back();
	return CurrentParticipant->GetFitness();
}

float GeneticTrainer::MeasureUniqueness(const std::shared_ptr<BlossomPlayer>& _Player)
{
	auto PlayerItr = std::find_if(Population.begin(), Population.end(), [&](const std::shared_ptr<BlossomPlayer>& _Comparison) { return _Player->GetIndex() == _Comparison->GetIndex(); });
	
	if (PlayerItr == Population.begin() || PlayerItr == Population.end() - 1 || PlayerItr == Population.end())
	{
		//std::cout << "P." << _Player->GetIndex() << " either a boundary player or it cannot be found...\n";
		return std::numeric_limits<float>::infinity();
	}

	//Order is reversed as the Population is sorted in descending fitness
	std::shared_ptr<Participant> Current = GetParticipant(_Player->GetIndex());
	std::shared_ptr<Participant> Inferior = GetParticipant((*(std::next(PlayerItr, 1)))->GetIndex());
	std::shared_ptr<Participant> Superior = GetParticipant((*(std::prev(PlayerItr, 1)))->GetIndex());
	Current->SetUniqueness(Superior->GetFitness() - Inferior->GetFitness());

	return Current->GetUniqueness();
}

float GeneticTrainer::MeasurePotential(const std::shared_ptr<BlossomPlayer>& _Player)
{
	auto PlayerItr = std::find_if(Population.begin(), Population.end(), [&](const std::shared_ptr<BlossomPlayer>& _Comparison) { return _Player->GetIndex() == _Comparison->GetIndex(); });

	if (PlayerItr == Population.begin() || PlayerItr == Population.end() - 1 || PlayerItr == Population.end())
		return std::numeric_limits<float>::infinity();

	std::shared_ptr<Participant> Current = GetParticipant(_Player->GetIndex());
	float AverageFitness = 0.0f;

	unsigned int NeighbourCount = 0;

	//Sampling towards superior
	auto SuperiorItr = PlayerItr;
	for (unsigned int Index = 0; Index < SamplingBreadth; Index++)
	{
		if (SuperiorItr == Population.begin())
			break;

		SuperiorItr = std::prev(SuperiorItr, 1);
		AverageFitness += GetParticipant((*(SuperiorItr))->GetIndex())->GetFitness();
		NeighbourCount++;
	}

	//Sampling towards inferior
	auto InferiorItr = PlayerItr;
	if (std::next(InferiorItr, 1) != Population.end())
	{
		for (unsigned int Index = 0; Index < SamplingBreadth; Index++)
		{
			InferiorItr = std::next(InferiorItr, 1);
			AverageFitness += GetParticipant((*(InferiorItr))->GetIndex())->GetFitness();
			NeighbourCount++;

			if (std::next(InferiorItr, 1) == Population.end())
				break;
		}
	}

	AverageFitness /= NeighbourCount;
	Current->SetPotential(AverageFitness - Current->GetFitness());
	return Current->GetPotential();
}

void GeneticTrainer::RankPlayer(const std::shared_ptr<BlossomPlayer>& _Player)
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
}

void GeneticTrainer::ArrangePlayers(std::vector<std::shared_ptr<BlossomPlayer>>& _Players)
{
	std::sort(_Players.begin(), _Players.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) { return GetParticipant(_First->GetIndex())->GetFitness() > GetParticipant(_Second->GetIndex())->GetFitness(); });
}

void GeneticTrainer::AddPlayersToHoF(unsigned int _Amt)
{
	for (unsigned int Index = 0; Index < _Amt; Index++)
	{
		auto TopItr = std::find_if(HoF.begin(), HoF.end(), [&](std::shared_ptr<Participant> _Participant) { return _Participant->GetOwner()->GetIndex() == RankingBoard[Index]->GetOwner()->GetIndex(); });

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
		else if (TopItr != HoF.end())
		{
			float PrevFitness = (*TopItr)->GetFitness();

			(*TopItr)->SetHandsWon(RankingBoard[Index]->GetHandsWon());
			(*TopItr)->SetHandsLost(RankingBoard[Index]->GetHandsLost());
			(*TopItr)->SetMoneyWon(RankingBoard[Index]->GetMoneyWon());
			(*TopItr)->SetMoneyLost(RankingBoard[Index]->GetMoneyLost());

			(*TopItr)->UpdateFitness();
			
			if ((*TopItr)->GetFitness() != PrevFitness)
				Writer->Overwrite(4, std::to_string(RankingBoard[Index]->GetOwner()->GetIndex()) + " " + std::to_string(PrevFitness), std::to_string(RankingBoard[Index]->GetOwner()->GetIndex()) + " " + std::to_string(RankingBoard[Index]->GetFitness()));
		}
	}
}

void GeneticTrainer::ArrangeHoF()
{
	std::sort(HoF.begin(), HoF.end(),
		[&](std::shared_ptr<Participant>& _First, std::shared_ptr<Participant>& _Second) { return _First->GetFitness() > _Second->GetFitness(); });
}

void GeneticTrainer::ClipHoF(unsigned int _Size)
{
	if (HoF.size() > _Size)
		HoF.erase(HoF.begin() + _Size, HoF.end());
}

const std::shared_ptr<Participant>& GeneticTrainer::GetParticipant(unsigned int _Index)
{
	for (auto const& Participant : RankingBoard)
	{
		if (Participant->GetOwner()->GetIndex() == _Index)
			return Participant;
	}

	return RankingBoard[0];
}

float GeneticTrainer::GetOverallFitness()
{
	float TotalFitness = 0.0;
	for (auto const& Participant : RankingBoard)
		TotalFitness += Participant->GetFitness();

	return TotalFitness / RankingBoard.size();
}

float GeneticTrainer::GetGenerationDiversity()
{
	//Distance to centroid
	std::array<float, 16> AverageThresholds{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	for (unsigned int Index = 0; Index < 16; Index++)
	{
		for (auto const& Player : Population)
			AverageThresholds[Index] += Player->GetAI().GetThresholds()[Index];

		AverageThresholds[Index] /= (float) Population.size();
 	}

	float TotalDist = 0.0f;

	for (auto const& Player : Population)
	{
		float CurrentDist = 0.0f;

		for (unsigned int Index = 0; Index < 16; Index++)
			CurrentDist += powf(Player->GetAI().GetThresholds()[Index] - AverageThresholds[Index], 2.0f);

		CurrentDist = powf(CurrentDist, 0.5f);
		TotalDist += abs(CurrentDist);
	}

	TotalDist /= (float) Population.size();

	return TotalDist;
}

std::shared_ptr<BlossomPlayer> GeneticTrainer::TournamentSelect_Fitness(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation)//, std::vector<std::shared_ptr<BlossomPlayer>>& _Parents)
{
	std::uniform_int_distribution<int> Distribution_Qualifier(0, PopulationSize - 1);

	std::vector<std::shared_ptr<BlossomPlayer>> Tournament;
	Tournament.reserve(TournamentSize);

	std::shared_ptr<BlossomPlayer> CurrentPlayer;

	for (unsigned int TourIndex = 0; TourIndex < TournamentSize; TourIndex++)
	{
		do { CurrentPlayer = _RefPopulation[Distribution_Qualifier(MTGenerator)]; } while (std::find(Population.begin(), Population.end(), CurrentPlayer) != Population.end());
		Tournament.push_back(CurrentPlayer);
	}

	std::sort(Tournament.begin(), Tournament.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) {return GetParticipant(_First->GetIndex())->GetFitness() > GetParticipant(_Second->GetIndex())->GetFitness(); });
	return Tournament[0];
}

std::shared_ptr<BlossomPlayer> GeneticTrainer::TournamentSelect_Uniqueness(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation)
{
	std::uniform_int_distribution<int> Distribution_Qualifier(0, PopulationSize - 1);

	std::vector<std::shared_ptr<BlossomPlayer>> Tournament;
	Tournament.reserve(TournamentSize);

	std::shared_ptr<BlossomPlayer> CurrentPlayer;

	for (unsigned int TourIndex = 0; TourIndex < TournamentSize; TourIndex++)
	{
		do { CurrentPlayer = _RefPopulation[Distribution_Qualifier(MTGenerator)]; } while (std::find(Population.begin(), Population.end(), CurrentPlayer) != Population.end());
		Tournament.push_back(CurrentPlayer);
	}

	std::sort(Tournament.begin(), Tournament.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) {return GetParticipant(_First->GetIndex())->GetUniqueness() > GetParticipant(_Second->GetIndex())->GetUniqueness(); });
	return Tournament[0];
}

std::shared_ptr<BlossomPlayer> GeneticTrainer::TournamentSelect_Potential(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation)
{
	std::uniform_int_distribution<int> Distribution_Qualifier(0, PopulationSize - 1);

	std::vector<std::shared_ptr<BlossomPlayer>> Tournament;
	Tournament.reserve(TournamentSize);

	std::shared_ptr<BlossomPlayer> CurrentPlayer;

	for (unsigned int TourIndex = 0; TourIndex < TournamentSize; TourIndex++)
	{
		do { CurrentPlayer = _RefPopulation[Distribution_Qualifier(MTGenerator)]; } while (std::find(Population.begin(), Population.end(), CurrentPlayer) != Population.end());
		Tournament.push_back(CurrentPlayer);
	}

	std::sort(Tournament.begin(), Tournament.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) {return GetParticipant(_First->GetIndex())->GetPotential() > GetParticipant(_Second->GetIndex())->GetPotential(); });
	return Tournament[0];
}

void GeneticTrainer::Crossover(const std::shared_ptr<BlossomPlayer>& _First, const std::shared_ptr<BlossomPlayer>& _Second, std::vector<std::shared_ptr<BlossomPlayer>>& _Results)
{
	//Binary Crossover
	/*std::array<float, 16> CombinedThresholds;
	std::uniform_int_distribution<int> Distribution_Crossover(0, 1);

	for (unsigned int ThrIndex = 0; ThrIndex < 16; ThrIndex++)
		CombinedThresholds[ThrIndex] = Distribution_Crossover(MTGenerator) == 0 ? _First->GetAI().GetThresholds()[ThrIndex] : _Second->GetAI().GetThresholds()[ThrIndex];

	_Results.push_back(std::move(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated)));
	_Results[0]->GetAI().SetThresholds(CombinedThresholds);*/

	//Multi-Set Crossover
	/*_Results.push_back(std::move(std::make_shared <BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated)));

	std::uniform_int_distribution<int> Distribution_ThreshSet(0, 1);

	for (unsigned int ThrIndex = 0; ThrIndex < 4; ThrIndex++)
	{
		switch (Distribution_ThreshSet(MTGenerator))
		{
		case 0:
			_Results[0]->GetAI().SetThresholdsByPhase((Phase)ThrIndex, _First->GetAI().GetThresholdsByPhase((Phase)ThrIndex));
			break;
		case 1:
			_Results[0]->GetAI().SetThresholdsByPhase((Phase)ThrIndex, _Second->GetAI().GetThresholdsByPhase((Phase)ThrIndex));
			break;
		}
	}*/

	//Single-Set Binary Crossover
	_Results.push_back(std::move(std::make_shared <BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated)));
	//PlayersGenerated++;
	_Results.push_back(std::move(std::make_shared <BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated + 1)));
	//PlayersGenerated++;

	std::uniform_int_distribution<int> Distribution_Set(0, 3);
	std::uniform_int_distribution<int> Distribution_Choice(0, 1);

	Phase TargetPhase = (Phase)Distribution_Set(MTGenerator);

	std::array<float, 4> ThresholdSet_0, ThresholdSet_1;

	for (unsigned int ThrIndex = 0; ThrIndex < 4; ThrIndex++)
	{
		switch (Distribution_Choice(MTGenerator))
		{
		case 0:
			ThresholdSet_0[ThrIndex] = _First->GetAI().GetThresholdsByPhase(TargetPhase)[ThrIndex];
			ThresholdSet_1[ThrIndex] = _Second->GetAI().GetThresholdsByPhase(TargetPhase)[ThrIndex];
			break;
		case 1:
			ThresholdSet_0[ThrIndex] = _Second->GetAI().GetThresholdsByPhase(TargetPhase)[ThrIndex];
			ThresholdSet_1[ThrIndex] = _First->GetAI().GetThresholdsByPhase(TargetPhase)[ThrIndex];
			break;
		}
	}

	for (unsigned int ResIndex = 0; ResIndex < _Results.size(); ResIndex++)
	{
		for (unsigned int PhIndex = 0; PhIndex < 4; PhIndex++)
		{
			if (ResIndex == 0)
			{
				if (PhIndex == (unsigned int)TargetPhase)
					_Results[ResIndex]->GetAI().SetThresholdsByPhase((Phase)PhIndex, ThresholdSet_0);
				else
					_Results[ResIndex]->GetAI().SetThresholdsByPhase((Phase)PhIndex, _First->GetAI().GetThresholdsByPhase((Phase)PhIndex));
			}
			else
			{
				if (PhIndex == (unsigned int)TargetPhase)
					_Results[ResIndex]->GetAI().SetThresholdsByPhase((Phase)PhIndex, ThresholdSet_1);
				else
					_Results[ResIndex]->GetAI().SetThresholdsByPhase((Phase)PhIndex, _Second->GetAI().GetThresholdsByPhase((Phase)PhIndex));
			}
		}
	}

	/*std::uniform_int_distribution<int> Distribution_Method(0, 1);
	unsigned int MethodIndex = Distribution_Method(MTGenerator);

	if (MethodIndex == 0)
	{
		std::cout << "Crossover: Single-set Binary Crossover\n";
		//Writer->WriteAt(0, "Crossover: Single-set Binary Crossover\n");

		//Single-Set Binary Crossover
		_Results.push_back(std::move(std::make_shared <BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated)));
		PlayersGenerated++;
		_Results.push_back(std::move(std::make_shared <BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated)));
		PlayersGenerated++;

		std::uniform_int_distribution<int> Distribution_Set(0, 3);
		std::uniform_int_distribution<int> Distribution_Choice(0, 1);

		Phase TargetPhase = (Phase)Distribution_Set(MTGenerator);

		std::array<float, 4> ThresholdSet_0, ThresholdSet_1;

		for (unsigned int ThrIndex = 0; ThrIndex < 4; ThrIndex++)
		{
			switch (Distribution_Choice(MTGenerator))
			{
			case 0:
				ThresholdSet_0[ThrIndex] = _First->GetAI().GetThresholdsByPhase(TargetPhase)[ThrIndex];
				ThresholdSet_1[ThrIndex] = _Second->GetAI().GetThresholdsByPhase(TargetPhase)[ThrIndex];
				break;
			case 1:
				ThresholdSet_0[ThrIndex] = _Second->GetAI().GetThresholdsByPhase(TargetPhase)[ThrIndex];
				ThresholdSet_1[ThrIndex] = _First->GetAI().GetThresholdsByPhase(TargetPhase)[ThrIndex];
				break;
			}
		}

		for (unsigned int ResIndex = 0; ResIndex < _Results.size(); ResIndex++)
		{
			for (unsigned int PhIndex = 0; PhIndex < 4; PhIndex++)
			{
				if (ResIndex == 0)
				{
					if (PhIndex == (unsigned int)TargetPhase)
						_Results[ResIndex]->GetAI().SetThresholdsByPhase((Phase)PhIndex, ThresholdSet_0);
					else
						_Results[ResIndex]->GetAI().SetThresholdsByPhase((Phase)PhIndex, _First->GetAI().GetThresholdsByPhase((Phase)PhIndex));
				}
				else
				{
					if (PhIndex == (unsigned int)TargetPhase)
						_Results[ResIndex]->GetAI().SetThresholdsByPhase((Phase)PhIndex, ThresholdSet_1);
					else
						_Results[ResIndex]->GetAI().SetThresholdsByPhase((Phase)PhIndex, _Second->GetAI().GetThresholdsByPhase((Phase)PhIndex));
				}
			}
		}
	}
	else if(MethodIndex == 1)
	{
		std::cout << "Crossover: Cross-set Binary Crossover\n";
		//Writer->WriteAt(0, "Crossover: Cross-set Binary Crossover\n");

		//Cross-Set Binary Crossover
		_Results.push_back(std::move(std::make_shared <BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated)));
		PlayersGenerated++;
		_Results.push_back(std::move(std::make_shared <BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated)));
		PlayersGenerated++;

		std::uniform_int_distribution<int> Distribution_Set(0, 3);
		std::uniform_int_distribution<int> Distribution_Choice(0, 1);

		Phase TargetPhase_0 = (Phase)Distribution_Set(MTGenerator), TargetPhase_1 = (Phase)Distribution_Set(MTGenerator);

		std::array<float, 4> ThresholdSet_0, ThresholdSet_1;

		for (unsigned int ThrIndex = 0; ThrIndex < 4; ThrIndex++)
		{
			switch (Distribution_Choice(MTGenerator))
			{
			case 0:
				ThresholdSet_0[ThrIndex] = _First->GetAI().GetThresholdsByPhase(TargetPhase_0)[ThrIndex];
				ThresholdSet_1[ThrIndex] = _Second->GetAI().GetThresholdsByPhase(TargetPhase_1)[ThrIndex];
				break;
			case 1:
				ThresholdSet_0[ThrIndex] = _Second->GetAI().GetThresholdsByPhase(TargetPhase_1)[ThrIndex];
				ThresholdSet_1[ThrIndex] = _First->GetAI().GetThresholdsByPhase(TargetPhase_0)[ThrIndex];
				break;
			}
		}

		for (unsigned int ResIndex = 0; ResIndex < _Results.size(); ResIndex++)
		{
			for (unsigned int PhIndex = 0; PhIndex < 4; PhIndex++)
			{
				if (ResIndex == 0)
				{
					if (PhIndex == (unsigned int)TargetPhase_0)
						_Results[ResIndex]->GetAI().SetThresholdsByPhase((Phase)PhIndex, ThresholdSet_0);
					else
						_Results[ResIndex]->GetAI().SetThresholdsByPhase((Phase)PhIndex, _First->GetAI().GetThresholdsByPhase((Phase)PhIndex));
				}
				else
				{
					if (PhIndex == (unsigned int)TargetPhase_1)
						_Results[ResIndex]->GetAI().SetThresholdsByPhase((Phase)PhIndex, ThresholdSet_1);
					else
						_Results[ResIndex]->GetAI().SetThresholdsByPhase((Phase)PhIndex, _Second->GetAI().GetThresholdsByPhase((Phase)PhIndex));
				}
			}
		}
	}	*/
}	

//Mutating a Threshold Set
bool GeneticTrainer::Mutate(std::shared_ptr<BlossomPlayer>& _Target)
{
	bool IsMutated = false;

	//Single Threshold
	/*std::uniform_real_distribution<float> Distribution_Mutation(-MutateDelta, MutateDelta);
	std::uniform_real_distribution<float> Distribution_Backtrack(0.0, MutateDelta);

	for (unsigned int PhaseIndex = 0; PhaseIndex < 4; PhaseIndex++)
	{
		if (HasMutationHappen())
		{
			std::array<float, 4> ThreshSet = _Target->GetAI().GetThresholdsByPhase(_Phase);
			float Delta = Distribution_Mutation(MTGenerator);

			Writer->WriteAt(0, "Mutated P." + std::to_string(_Target->GetIndex()) + " Threshold set " + std::to_string((int)_Phase) + " Index " + std::to_string(_Index) + " with a delta of " + std::to_string(Delta) + "\n");

			ThreshSet[_Index] = ThreshSet[_Index] + Delta;
			if (ThreshSet[_Index] < 0)
			{
				Delta = Distribution_Backtrack(MTGenerator);
				ThreshSet[_Index] = Delta;
			}

			_Target->GetAI().SetThresholdsByPhase(_Phase, ThreshSet);
		}
	}*/

	//Entire Set
	std::uniform_real_distribution<float> Distribution_Mutation(-MutateDelta, MutateDelta);
	std::uniform_real_distribution<float> Distribution_Backtrack(0.0, MutateDelta);
	
	for (unsigned int PhaseIndex = 0; PhaseIndex < 4; PhaseIndex++)
	{
		if (HasMutationHappen())
		{
			std::array<float, 4> ThreshSet = _Target->GetAI().GetThresholdsByPhase((Phase) PhaseIndex);

			for (auto& Thresh : ThreshSet)
			{
				Thresh = Thresh + Distribution_Mutation(MTGenerator);
				if (Thresh < 0)
					Thresh = Distribution_Backtrack(MTGenerator);
			}

			_Target->GetAI().SetThresholdsByPhase((Phase) PhaseIndex, ThreshSet);

			IsMutated = true;
		}
	}

	return IsMutated;
}

std::shared_ptr<BlossomPlayer> GeneticTrainer::Adapt(const std::shared_ptr<BlossomPlayer>& _Target, const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation)
{
	auto PlayerItr = std::find_if(_RefPopulation.begin(), _RefPopulation.end(), [&](const std::shared_ptr<BlossomPlayer>& _Comparison) { return _Target->GetIndex() == _Comparison->GetIndex(); });
	std::shared_ptr<BlossomPlayer> SuperiorNeighbour = _Target;

	auto MinItr = PlayerItr, MaxItr = PlayerItr;
	for (unsigned int Index = 0; Index < SamplingBreadth; Index++)
	{
		MinItr = std::prev(MinItr,1);
		
		if (MinItr == _RefPopulation.begin())
			break;
	}
	for (unsigned int Index = 0; Index < SamplingBreadth; Index++)
	{
		if (std::next(MaxItr, 1) == _RefPopulation.end())
			break;

		MaxItr = std::next(MaxItr, 1);
	}

	for (auto Itr = MinItr; Itr <= MaxItr; Itr++)
	{
		if (Itr == _RefPopulation.end())
			break;

		if (GetParticipant((*(Itr))->GetIndex())->GetFitness() > GetParticipant(SuperiorNeighbour->GetIndex())->GetFitness())
			SuperiorNeighbour = *(Itr);
	}

	std::shared_ptr<BlossomPlayer> ImprovedPlayer = std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated);
	ImprovedPlayer->GetAI().SetThresholds(SuperiorNeighbour->GetAI().GetThresholds());

	std::cout << "Newly adapted player P." << ImprovedPlayer->GetIndex() << " is created based on P." << SuperiorNeighbour->GetIndex() << "\n";
	Writer->WriteAt(0, "Newly adapted player P." + std::to_string(ImprovedPlayer->GetIndex()) + " is created based on P." + std::to_string(SuperiorNeighbour->GetIndex()) + "\n");

	PlayersGenerated++;
	return ImprovedPlayer;
}

void GeneticTrainer::EvaluateMutateRate()
{
	//Custom Diversity-based Adaptive Mutation
	/*float Sensitivity = 0.5f;//0.1f;
	float Diversity_Min = 0.75f, Diversity_Max = 1.3f;
	float Diversity_Current = GetGenerationDiversity();

	if (MutatePhase == 0)
	{
		MutateRate = MutateRate * (1 + (Sensitivity * ((Diversity_Min - Diversity_Current) / Diversity_Current)));
		MutatePhase = Diversity_Current <= Diversity_Min ? 1 : 0;
	}
	else
	{
		MutateRate = MutateRate * (1 + (Sensitivity * ((Diversity_Max - Diversity_Current) / Diversity_Current)));
		MutatePhase = Diversity_Current >= Diversity_Max ? 0 : 1;
	}

	MutateRate = std::max(0.0f, std::min(MutateRate, 0.5f));*/

	//Diversity-based Adaptive Mutation
	//float Sensitivity = 0.225f;//0.3f;
	//float TargetDiversity = 0.8f;
	//float CurrentDiversity = GetGenerationDiversity();

	//MutateRate = MutateRate * (1 + (Sensitivity * ((TargetDiversity - CurrentDiversity) / CurrentDiversity)));
	//MutateRate = std::max(0.0f, std::min(MutateRate, 0.5f));

	//Gaussian Distribution
	float a = 2.5f, b = 0.5f, c = 1.5075f;//float a = 2.5f, b = 0.5f, c = 0.15f;
	float x = (float) Generation / (float) GenerationLimit;
	float e = std::exp(1.0f);
	MutateRate = pow((a * e), -(pow((x - b), 2) / (2 * pow(c, 2)))) - 0.9f;//pow((a * e), -(pow((x - b), 2) / (2 * pow(c, 2))));

	//Oscillating Sine Wave
	/*float Freq = 48.7f;
	float HeightOffset = 0.5f;
	float GenRatio = (float)Generation / (float)GenerationLimit;
	MutateRate = (sin(Freq * sqrt(GenRatio))) / 2.0f + HeightOffset;

	MutateRate = std::max(0.0f, std::min(MutateRate, 1.0f));*/

	Writer->WriteAt(3, (float)Generation, MutateRate);
}

//Mutating a specific threshold
/*void GeneticTrainer::Mutate(std::shared_ptr<BlossomPlayer>& _Target, Phase _Phase, unsigned int _ParaIndex)
{
	std::uniform_real_distribution<float> Distribution_Mutation(-MutateDelta, MutateDelta);

	float MutatedThreshold = _Target->GetAI().GetThresholdsByPhase(_Phase)[_ParaIndex] + Distribution_Mutation(MTGenerator);
	
	if (MutatedThreshold < 0)
	{
		std::uniform_real_distribution<float> Distribution_Backtrack(0.0, MutateDelta);
		MutatedThreshold = Distribution_Backtrack(MTGenerator);
	}

	_Target->GetAI().SetThresholdByPhase(_Phase, _ParaIndex, MutatedThreshold);
}*/

bool GeneticTrainer::HasCrossoverHappen()
{
	std::uniform_real_distribution<float> Distribution_CrossChance(0.0f, 1.0f);
	return Distribution_CrossChance(MTGenerator) <= CrossoverRate ? true : false;
}

bool GeneticTrainer::HasMutationHappen()
{
	std::uniform_real_distribution<float> Distribution_MutateChance(0.0f, 1.0f);
	return Distribution_MutateChance(MTGenerator) <= MutateRate ? true : false;
}

void GeneticTrainer::ReproducePopulation()
{
	//std::cout << "\nReproducing population...\n";
	Writer->WriteAt(0, "\nReproducing population...\n");

	EvaluateMutateRate();

	//Reproduction without Elitism
	/*std::vector<std::shared_ptr<BlossomPlayer>> PopulationReference(Population.begin(), Population.end());
	Population.clear();

	std::vector<std::shared_ptr<BlossomPlayer>> Parents;
	std::shared_ptr<BlossomPlayer> Child;

	//Generate the same amount of children as the current generation for the next
	for (unsigned int ThrIndex = 0; ThrIndex < PopulationSize; ThrIndex++)
	{
		//Select Parents
		TournamentSelect_Fitness(PopulationReference, Parents);

		//Cross-over
		Crossover(Parents[0], Parents[1], Child);

		//Gaussian Mutation
		//float a = 2.5f, b = 0.5f, c = 0.15f;
		//float x = (float)Generation / (float)GenerationLimit;
		//float e = std::exp(1.0f);
		//float MutatingRate = pow((a * e), -(pow((x - b), 2) / (2 * pow(c, 2))));

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
					Mutate(Child, (Phase)PhaseIndex, ParaIndex);
			}
		}

		Population.push_back(Child);

		Writer->WriteAt(0, "Parents: P." + std::to_string(Parents[0]->GetIndex()) + " & P." + std::to_string(Parents[1]->GetIndex()) + " \n");
		Writer->WriteAt(0, "Child " + std::to_string(ThrIndex) + ": P." + std::to_string(PlayersGenerated) + " (" + GetThresholdsStr(Child) + ")\n");
		Parents.clear();
	}*/

	//Reproduction with Elitism
	/*std::vector<std::shared_ptr<BlossomPlayer>> PopulationReference(Population.begin(), Population.end());
	Population.erase(Population.begin() + ElitesLimit, Population.end());

	float EliteAverFitness = 0.0f;

	for (unsigned int Index = 0; Index < ElitesLimit; Index++)
	{
		std::cout << "Elite: P." << Population[Index]->GetIndex() << "\n";
		Writer->WriteAt(0, "Elite: P." + std::to_string(Population[Index]->GetIndex()) + "\n");

		EliteAverFitness += GetParticipant(Population[Index]->GetIndex())->GetFitness();
	}

	EliteAverFitness /= (float) ElitesLimit;
	Writer->WriteAt(7, Generation, EliteAverFitness);

	std::vector<std::shared_ptr<BlossomPlayer>> Parents;
	std::vector<std::shared_ptr<BlossomPlayer>> Children;
	std::shared_ptr<BlossomPlayer> Reference;

	std::uniform_int_distribution<int> Distribution_Phase(0, 3);
	std::uniform_int_distribution<int> Distribution_Binary(0, 1);

	while(Population.size() < PopulationSize)
	{
		Parents.clear();
		Children.clear();

		if (HasCrossoverHappen())
		{
			while (Parents.size() < 2)
			{
				Reference = TournamentSelect_Fitness(PopulationReference);

				if (std::find_if(Parents.begin(), Parents.end(), [&](std::shared_ptr<BlossomPlayer> _Parent) { return _Parent->GetIndex() == Reference->GetIndex(); }) == Parents.end())
					Parents.push_back(Reference);
			}

			std::cout << "Parents: P." << Parents[0]->GetIndex() << " & P." << Parents[1]->GetIndex() << "...\n";
			Writer->WriteAt(0, "Parents: P." + std::to_string(Parents[0]->GetIndex()) + " & P." + std::to_string(Parents[1]->GetIndex()) + "\n");

			Crossover(Parents[0], Parents[1], Children);

			for (unsigned int Index = 0; Index < Children.size(); Index++)
			{
				if (Population.size() >= PopulationSize)
					break;

				std::array<float, 16> InitialThresholds = Children[Index]->GetAI().GetThresholds();

				Mutate(Children[Index]);
				std::cout << "Child of Crossover: P." << Children[Index]->GetIndex() << " (" << GetThresholdsStr(Children[Index]) << ")\n";
				Writer->WriteAt(0, "Child of Crossover: P." + std::to_string(Children[Index]->GetIndex()) + " (" + GetThresholdsStr(Children[Index]) + ")\n");

				bool HasItMutated = false;
				for (unsigned int ThrIndex = 0; ThrIndex < 16; ThrIndex++)
				{
					if (Children[Index]->GetAI().GetThresholds()[ThrIndex] != InitialThresholds[ThrIndex])
					{
						HasItMutated = true;
						break;
					}
				}

				if (HasItMutated)
				{
					std::cout << "Mutation was made, the crossover Child P." << Children[Index]->GetIndex() << " shall be evaluated...\n";
					Writer->WriteAt(0, "Mutation was made, the crossover Child P." + std::to_string(Children[Index]->GetIndex()) + " shall be evaluated...\n");

					//If mutated child scores lower than the worst performing player in the generation, reject it and select a random parent to pass onto the next generation
					float ChildFitness = MeasureFitness(Children[Index]);
					float WorstFitness = GetParticipant(PopulationReference[PopulationReference.size() - 1]->GetIndex())->GetFitness();

					std::cout << "Child Fitness: " << ChildFitness << " / Worst Fitness: " << WorstFitness << "\n";
					Writer->WriteAt(0, "Child Fitness: " + std::to_string(ChildFitness) + " / Worst Fitness: " + std::to_string(WorstFitness) + "\n");

					if (ChildFitness >= WorstFitness)//MeasureFitness(Child) >= GetParticipant(Population[Population.size() - 1]->GetIndex())->GetFitness())
					{
						Population.push_back(Children[Index]);
						std::cout << "Child P." << Children[Index]->GetIndex() << " will pass onto next generation...\n";
						Writer->WriteAt(0, "Child P." + std::to_string(Children[Index]->GetIndex()) + " will pass onto next generation...\n");
					}
					else
					{
						std::cout << "The index of supposed child: P." << Children[Index]->GetIndex() << " / New Child Index: " << (PlayersGenerated - 2 + Index) << "\n";
						Population.push_back(std::move(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated - 2 + Index)));

						for (unsigned int PhaseIndex = 0; PhaseIndex < 4; PhaseIndex++)
							Population[Population.size() - 1]->GetAI().SetThresholdsByPhase((Phase)PhaseIndex, Parents[Index]->GetAI().GetThresholdsByPhase((Phase)PhaseIndex));

						std::cout << "Child scores lower than the worst player, Parent is selected to pass on...\n";
						Writer->WriteAt(0, "Child scores lower than the worst player, Parent is selected to pass on...\n");
					}
				}
				else
				{
					Population.push_back(Children[Index]);
					std::cout << "No mutation was made, the crossover Child P." << Children[Index]->GetIndex() << " will be passed into next generation...\n";
					Writer->WriteAt(0, "No mutation was made, the crossover Child P." + std::to_string(Children[Index]->GetIndex()) + " will be passed into next generation...\n");
					std::cout << "Child P." << Children[Index]->GetIndex() << " will pass onto next generation...\n";
					Writer->WriteAt(0, "Child P." + std::to_string(Children[Index]->GetIndex()) + " will pass onto next generation...\n");
				}

				
			}
		}
		else
		{
			Children.push_back(std::move(std::make_shared <BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated)));
			Reference = TournamentSelect_Fitness(PopulationReference);

			std::cout << "Reference: P." << Reference->GetIndex() << "\n";

			for (unsigned int Index = 0; Index < 4; Index++)
				Children[0]->GetAI().SetThresholdsByPhase((Phase)Index, Reference->GetAI().GetThresholdsByPhase((Phase)Index));

			std::array<float, 16> InitialThresholds = Children[0]->GetAI().GetThresholds();

			Mutate(Children[0]);
			std::cout << "Child W/O Crossover: P." << Children[0]->GetIndex() << " (" << GetThresholdsStr(Children[0]) << ")\n";
			Writer->WriteAt(0, "Child W/O Crossover: P." + std::to_string(Children[0]->GetIndex()) + " (" + GetThresholdsStr(Children[0]) + ")\n");

			bool HasItMutated = false;
			for (unsigned int ThrIndex = 0; ThrIndex < 16; ThrIndex++)
			{
				if (Children[0]->GetAI().GetThresholds()[ThrIndex] != InitialThresholds[ThrIndex])
				{
					HasItMutated = true;
					break;
				}
			}

			if (HasItMutated)
			{
				std::cout << "Mutation was made, the crossover Child P." << Children[0]->GetIndex() << " shall be evaluated...\n";
				Writer->WriteAt(0, "Mutation was made, the crossover Child P." + std::to_string(Children[0]->GetIndex()) + " shall be evaluated...\n");

				float ChildFitness = MeasureFitness(Children[0]);
				float WorstFitness = GetParticipant(PopulationReference[PopulationReference.size() - 1]->GetIndex())->GetFitness();

				std::cout << "Child Fitness: " << ChildFitness << " / Worst Fitness: " << WorstFitness << "\n";
				Writer->WriteAt(0, "Child Fitness: " + std::to_string(ChildFitness) + " / Worst Fitness: " + std::to_string(WorstFitness) + "\n");

				if (ChildFitness >= WorstFitness)//MeasureFitness(Children[0]) >= GetParticipant(Population[Population.size() - 1]->GetIndex())->GetFitness())
				{
					Population.push_back(Children[0]);
					std::cout << "Child P." << Children[0]->GetIndex() << " will pass onto next generation...\n";
					Writer->WriteAt(0, "Child P." + std::to_string(Children[0]->GetIndex()) + " will pass onto next generation...\n");
				}
				else
				{
					std::cout << "The index of supposed child: P." << Children[0]->GetIndex() << " / New Child Index: " << (PlayersGenerated) << "\n";
					Population.push_back(std::move(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated)));
					for (unsigned int PhaseIndex = 0; PhaseIndex < 4; PhaseIndex++)
						Population[Population.size() - 1]->GetAI().SetThresholdsByPhase((Phase)PhaseIndex, Reference->GetAI().GetThresholdsByPhase((Phase)PhaseIndex));

					std::cout << "Child scores lower than the worst player, Parent is selected to pass on...\n";
					Writer->WriteAt(0, "Child scores lower than the worst player, Parent is selected to pass on...\n");
				}
			}
			else
			{
				Population.push_back(Children[0]);
				std::cout << "No mutation was made, the crossover Child P." << Children[0]->GetIndex() << " will be passed into next generation...\n";
				Writer->WriteAt(0, "No mutation was made, the crossover Child P." + std::to_string(Children[0]->GetIndex()) + " will be passed into next generation...\n");
				std::cout << "Child P." << Children[0]->GetIndex() << " will pass onto next generation...\n";
				Writer->WriteAt(0, "Child P." + std::to_string(Children[0]->GetIndex()) + " will pass onto next generation...\n");
			}

			PlayersGenerated++;		
		}
	}

	for (unsigned int Index = ElitesLimit; Index < Population.size(); Index++)
	{
		RankingBoard[Index]->Refresh();
		RankingBoard[Index]->SetOwner(Population[Index]);
	}
	
	Generation++;*/

	//Reproduction with GARS
	std::vector<std::shared_ptr<BlossomPlayer>> PopulationReference(Population.begin(), Population.end());
	Population.erase(Population.begin() + ElitesLimit, Population.end());

	std::vector<std::shared_ptr<BlossomPlayer>> Parents;
	std::vector<std::shared_ptr<BlossomPlayer>> Children;
	std::shared_ptr<BlossomPlayer> Reference;

	//Elites
	//std::cout << "\nElites: ";
	Writer->WriteAt(0, "\nElites: ");
	float EliteAverageFitness = 0.0f;
	for (unsigned int Index = 0; Index < ElitesLimit; Index++)
	{
		EliteAverageFitness += GetParticipant(Population[Index]->GetIndex())->GetFitness();

		//std::cout << "P." << Population[Index]->GetIndex() << " ";
		Writer->WriteAt(0, "P." + std::to_string(Population[Index]->GetIndex()) + " ");
	}
	EliteAverageFitness /= (float)ElitesLimit;
	Writer->WriteAt(7, Generation, EliteAverageFitness);

	//NRA
	//std::cout << "\n\nGenerating Offspring for NRA...\n";
	Writer->WriteAt(0, "\n\nGenerating Offspring for NBA...\n");

	for (unsigned int Index = 0; Index < PopulationSize - ReserveSize - ElitesLimit; Index++)
	{
		if (Population.size() >= (PopulationSize - ReserveSize) || Population.size() >= PopulationSize)
			break;

		Parents.clear();
		Children.clear();

		while (Parents.size() < 2)
		{
			Reference = TournamentSelect_Fitness(PopulationReference);

			if (std::find_if(Parents.begin(), Parents.end(), [&](std::shared_ptr<BlossomPlayer> _Parent) { return _Parent->GetIndex() == Reference->GetIndex(); }) == Parents.end())
				Parents.push_back(Reference);
		}

		if (HasCrossoverHappen())
		{
			Crossover(Parents[0], Parents[1], Children);

			for (auto& Child : Children)
			{
				if (Population.size() >= (PopulationSize - ReserveSize) || Population.size() >= PopulationSize)
					break;

				if (Mutate(Child))
				{
					//std::cout << "Mutation was made...	|	";
					Writer->WriteAt(0, "Mutation was made...	|	");
				}

				Population.push_back(Child);
				PlayersGenerated++;

				//std::cout << "P." << Child->GetIndex() << " (Crossover - P." << Parents[0]->GetIndex() << ", P." << Parents[1]->GetIndex() << ")\n";
				Writer->WriteAt(0, "P." + std::to_string(Child->GetIndex()) + " (Crossover - P." + std::to_string(Parents[0]->GetIndex()) + ", P." + std::to_string(Parents[1]->GetIndex()) + ")\n");
			}

			for (auto const& Parent : Parents)
			{
				if (std::find_if(SelectionTable.begin(), SelectionTable.end(), [&](unsigned int _Index) { return Parent->GetIndex() == _Index; }) == SelectionTable.end())
					SelectionTable.push_back(Parent->GetIndex());
			}
		}
		else
		{
			Children.push_back(std::move(std::make_shared <BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated)));
			Reference = Parents[0];

			for (unsigned int Index = 0; Index < 4; Index++)
				Children[0]->GetAI().SetThresholdsByPhase((Phase)Index, Reference->GetAI().GetThresholdsByPhase((Phase)Index));

			std::array<float, 16> InitialThresholds = Children[0]->GetAI().GetThresholds();

			if(Mutate(Children[0]))
			{
				//std::cout << "Mutation was made...	|	";
				Writer->WriteAt(0, "Mutation was made...	|	");
			}

			Population.push_back(Children[0]);

			//std::cout << "P." << Children[0]->GetIndex() << " (W/O Crossover - P." << Reference->GetIndex() << ")\n";
			Writer->WriteAt(0, "P." + std::to_string(Children[0]->GetIndex()) + " (W/O Crossover - P." + std::to_string(Reference->GetIndex()) + ")\n");

			if (std::find_if(SelectionTable.begin(), SelectionTable.end(), [&](unsigned int _Index) { return Parents[0]->GetIndex() == _Index; }) == SelectionTable.end())
				SelectionTable.push_back(Parents[0]->GetIndex());

			PlayersGenerated++;
		}
	}

	//RA
	//std::cout << "\nGenerating Offsprings for RA...\n";
	Writer->WriteAt(0, "\nGenerating Offsprings for RA...\n");

	std::uniform_real_distribution<float> Distribution_Adaptation(0.0f, 1.0f);
	bool IsAdapting = Distribution_Adaptation(MTGenerator) < AdaptationRate;

	for (unsigned int Index = 0; Index < ReserveSize; Index++)
	{
		if (Population.size() >= PopulationSize)
			break;

		Parents.clear();
		Children.clear();

		if (IsAdapting)
		{
			while (Parents.size() < 1)
			{
				Reference = TournamentSelect_Potential(PopulationReference);

				if (std::find_if(Parents.begin(), Parents.end(), [&](std::shared_ptr<BlossomPlayer> _Parent) { return _Parent->GetIndex() == Reference->GetIndex(); }) == Parents.end() &&
					std::find(SelectionTable.begin(), SelectionTable.end(), Reference->GetIndex()) == SelectionTable.end())
					Parents.push_back(Reference);
			}

			Reference = Adapt(Parents[0], PopulationReference);
			Population.push_back(Reference);
			
			//std::cout << "P." << Reference->GetIndex() << " (Adaptation - P." << Parents[0]->GetIndex() << ")\n";
			Writer->WriteAt(0, "P." + std::to_string(Reference->GetIndex()) + " (Adaptation - P." + std::to_string(Parents[0]->GetIndex()) + ")\n");
		}
		else
		{
			while (Parents.size() < 2)
			{
				Reference = TournamentSelect_Uniqueness(PopulationReference);

				if (std::find_if(Parents.begin(), Parents.end(), [&](std::shared_ptr<BlossomPlayer> _Parent) { return _Parent->GetIndex() == Reference->GetIndex(); }) == Parents.end() &&
					std::find(SelectionTable.begin(), SelectionTable.end(), Reference->GetIndex()) == SelectionTable.end())
					Parents.push_back(Reference);
			}

			if (HasCrossoverHappen())
			{
				Crossover(Parents[0], Parents[1], Children);

				for (auto& Child : Children)
				{
					if (Mutate(Child))
					{
						//std::cout << "Mutation was made...	|	";
						Writer->WriteAt(0, "Mutation was made...	|	");
					}

					Population.push_back(Child);
					PlayersGenerated++;

					//std::cout << "P." << Child->GetIndex() << " (Crossover - P." << Parents[0]->GetIndex() << ", P." << Parents[1]->GetIndex() << ")\n";
					Writer->WriteAt(0, "P." + std::to_string(Child->GetIndex()) + " (Crossover - P." + std::to_string(Parents[0]->GetIndex()) + ", P." + std::to_string(Parents[1]->GetIndex()) + ")\n");

					if (Population.size() >= PopulationSize)
						break;
				}
			}
			else
			{
				Children.push_back(std::move(std::make_shared <BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated)));
				Reference = Parents[0];

				for (unsigned int Index = 0; Index < 4; Index++)
					Children[0]->GetAI().SetThresholdsByPhase((Phase)Index, Reference->GetAI().GetThresholdsByPhase((Phase)Index));

				if (Mutate(Children[0]))
				{
					//std::cout << "Mutation was made...	|	";
					Writer->WriteAt(0, "Mutation was made...	|	");
				}

				Population.push_back(Children[0]);

				//std::cout << "P." << Children[0]->GetIndex() << " (W/O Crossover - P." << Reference->GetIndex() << ")\n";
				Writer->WriteAt(0, "P." + std::to_string(Children[0]->GetIndex()) + " (W/O Crossover - P." + std::to_string(Reference->GetIndex()) + ")\n");

				PlayersGenerated++;
			}
		}
	}

	//Re-allocate the players to their respective participants within RankingBoard
	for (unsigned int Index = ElitesLimit; Index < Population.size(); Index++)
	{
		RankingBoard[Index]->Refresh();
		RankingBoard[Index]->SetOwner(Population[Index]);
	}

	Generation++;
}

void GeneticTrainer::CullPopulation()
{
	//Culling players with lowest uniqueness
	/*//Sort RankiingBoard by ascending uniqueness
	std::sort(RankingBoard.begin(), RankingBoard.end(), [&](std::shared_ptr<Participant> _First, std::shared_ptr<Participant> _Second) { return _First->GetUniqueness() < _Second->GetUniqueness(); });

	float CullRatio = 0.25f;
	unsigned int TargetIndex = (unsigned int)(CullRatio * (float)PopulationSize);

	//Erase and replace the lowest uniqueness players within RankingBoard and Population
	for (unsigned int Index = 0; Index < TargetIndex; Index++)
	{
		auto Itr = std::find_if(Population.begin(), Population.end(), [&](std::shared_ptr<BlossomPlayer> _Player) { return RankingBoard[Index]->GetOwner()->GetIndex() == _Player->GetIndex(); });

		if (Itr != Population.end())
		{
			unsigned int ErasedIndex = (*(Itr))->GetIndex();

			Population.erase(Itr);
			Population.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated));

			GetParticipant(ErasedIndex)->Refresh();
			GetParticipant(ErasedIndex)->SetOwner(Population[Population.size() - 1]);

			PlayersGenerated++;
		}
	}

	//Sort RankingBoard by descending fitness
	std::sort(RankingBoard.begin(), RankingBoard.end(), [&](std::shared_ptr<Participant> _First, std::shared_ptr<Participant> _Second) { return _First->GetFitness() > _Second->GetFitness(); });*/

	//Culling players with lowest fitness
	//Writer->WriteAt(0, "Culling players with lowest fitness...\n");

	//Sort RankingBoard by ascending fitness
	std::sort(RankingBoard.begin(), RankingBoard.end(), [&](std::shared_ptr<Participant> _First, std::shared_ptr<Participant> _Second) { return _First->GetFitness() < _Second->GetFitness(); });

	Writer->WriteAt(0, "Population:\n");
	for (auto const& Player : Population)
		Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + std::to_string(GetParticipant(Player->GetIndex())->GetFitness()) + "\n");
	Writer->WriteAt(0, "\n\n");

	Writer->WriteAt(0, "RankingBoard by ascending fitness:\n");
	for (auto const& Participant : RankingBoard)
		Writer->WriteAt(0, "P." + std::to_string(Participant->GetOwner()->GetIndex()) + ": " + std::to_string(Participant->GetFitness()) + "\n");
	Writer->WriteAt(0, "\n\n");

	float CullRatio = 0.25f;
	unsigned int TargetIndex = (unsigned int)(CullRatio * (float)PopulationSize);

	Writer->WriteAt(0, "CullRatio: " + std::to_string(CullRatio) + " / TargetIndex: " + std::to_string(TargetIndex) + "\n");

	//Erase and replace the lowest fitness players within RankingBoard and Population
	for (unsigned int Index = 0; Index < TargetIndex; Index++)
	{
		auto Itr = std::find_if(Population.begin(), Population.end(), [&](std::shared_ptr<BlossomPlayer> _Player) { return RankingBoard[Index]->GetOwner()->GetIndex() == _Player->GetIndex(); });

		if (Itr != Population.end())
		{
			unsigned int ErasedIndex = (*(Itr))->GetIndex();

			Population.erase(Itr);
			Population.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated));

			GetParticipant(ErasedIndex)->Refresh();
			GetParticipant(ErasedIndex)->SetOwner(Population[Population.size() - 1]);

			PlayersGenerated++;
		}
	}

	//Sort RankingBoard by descending fitness
	std::sort(RankingBoard.begin(), RankingBoard.end(), [&](std::shared_ptr<Participant> _First, std::shared_ptr<Participant> _Second) { return _First->GetFitness() > _Second->GetFitness(); });

	Writer->WriteAt(0, "Post-cull Population:\n");
	for (auto const& Player : Population)
		Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + std::to_string(GetParticipant(Player->GetIndex())->GetFitness()) + "\n");
	Writer->WriteAt(0, "\n\n");

	Writer->WriteAt(0, "Post-cull RankingBoard by descending fitness:\n");
	for (auto const& Participant : RankingBoard)
		Writer->WriteAt(0, "P." + std::to_string(Participant->GetOwner()->GetIndex()) + ": " + std::to_string(Participant->GetFitness()) + "\n");
	Writer->WriteAt(0, "\n\n");

	CullCount++;
	Generation++;
}

void GeneticTrainer::NukePopulation()
{
	Writer->WriteAt(0, "Nuking players and leaving " + std::to_string(ElitesLimit) + " elites in Population...\n");

	Writer->WriteAt(0, "Population:\n");
	for (auto const& Player : Population)
		Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + std::to_string(GetParticipant(Player->GetIndex())->GetFitness()) + "\n");
	Writer->WriteAt(0, "\n\n");

	Writer->WriteAt(0, "RankingBoard:\n");
	for (auto const& Participant : RankingBoard)
		Writer->WriteAt(0, "P." + std::to_string(Participant->GetOwner()->GetIndex()) + ": " + std::to_string(Participant->GetFitness()) + "\n");
	Writer->WriteAt(0, "\n\n");

	//Erase and replace all players within Population except Elites
	for (unsigned int Index = ElitesLimit; Index < PopulationSize; Index++)
	{
		auto Itr = std::find_if(Population.begin(), Population.end(), [&](std::shared_ptr<BlossomPlayer> _Player) { return _Player->GetIndex() == RankingBoard[Index]->GetOwner()->GetIndex(); });

		if (Itr != Population.end())
		{
			unsigned int ErasedIndex = (*(Itr))->GetIndex();

			Population.erase(Itr);
			Population.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated));

			GetParticipant(ErasedIndex)->Refresh();
			GetParticipant(ErasedIndex)->SetOwner(Population[Population.size() - 1]);

			PlayersGenerated++;
		}
	}

	//Sort RankingBoard by descending fitness
	std::sort(RankingBoard.begin(), RankingBoard.end(), [&](std::shared_ptr<Participant> _First, std::shared_ptr<Participant> _Second) { return _First->GetFitness() > _Second->GetFitness(); });

	Writer->WriteAt(0, "Post-nuke Population:\n");
	for (auto const& Player : Population)
		Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + std::to_string(GetParticipant(Player->GetIndex())->GetFitness()) + "\n");
	Writer->WriteAt(0, "\n\n");

	Writer->WriteAt(0, "Post-nuke RankingBoard:\n");
	for (auto const& Participant : RankingBoard)
		Writer->WriteAt(0, "P." + std::to_string(Participant->GetOwner()->GetIndex()) + ": " + std::to_string(Participant->GetFitness()) + "\n");
	Writer->WriteAt(0, "\n\n");

	CullCount = 0;
	Generation++;
}

bool GeneticTrainer::HasPopulationStagnate()
{
	std::cout << "Current Generation: " << Generation << " / StagnateInterval: " << StagnateInterval << " / StagnateLength: " << StagnateLength << "\n";

	if (Generation == 0 || Generation % StagnateInterval != 0)
		return false;

	float CurrentFitness = GetOverallFitness();
	float InitialFitness = GenerationAverFitness[Generation - StagnateLength];

	Writer->WriteAt(0, "\nChecking for Stagnation (Generation: " + std::to_string(Generation) + " / StaganteInterval: " + std::to_string(StagnateInterval) + " / StagnateLength: " + std::to_string(StagnateLength) + ")...\n");
	Writer->WriteAt(0, "Generation " + std::to_string(Generation) + "'s Fitness: " + std::to_string(CurrentFitness) + "\n");
	Writer->WriteAt(0, "Reference Generation " + std::to_string(Generation - StagnateInterval) + "'s Fitness: " + std::to_string(InitialFitness) + "\n");
	Writer->WriteAt(0, "Absolute Diff: " + std::to_string(std::abs(CurrentFitness - InitialFitness)) + " / Min Fitness Diff: " + std::to_string(MinFitnessDiff) + "\n\n");

	if (std::abs(CurrentFitness - InitialFitness) < MinFitnessDiff)
		return true;

	return false;
}

std::string GeneticTrainer::GetPopulationContentStr()
{
	std::string PopuStr = "";

	for (auto const& Player : Population)
		PopuStr += "P." + std::to_string(Player->GetIndex()) + " ";

	return PopuStr;
}

std::string GeneticTrainer::GetThresholdsStr(const std::shared_ptr<BlossomPlayer>& _Target)
{
	std::string ThrStr = "";

	for (unsigned int ThrIndex = 0; ThrIndex < 16; ThrIndex++)
		ThrStr += std::to_string(_Target->GetAI().GetThresholds()[ThrIndex]) + "  ";

	return ThrStr;
}

void GeneticTrainer::SetSpecs(unsigned int _PopulationSize, unsigned int _GenerationLimit, unsigned int _ToursPerGen)
{
	PopulationSize = _PopulationSize;
	GenerationLimit = _GenerationLimit;
	ToursPerGen = _ToursPerGen;
	ElitesLimit = (unsigned int)(ElitesRatio * (float)PopulationSize);
	ReserveSize = (unsigned int)(ReserveRatio * (float)PopulationSize);
}