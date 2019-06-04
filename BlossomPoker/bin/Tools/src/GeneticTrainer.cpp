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
#include "../../Player/inc/CallRaiser.h"
#include "../../Table/inc/Tournament.h"

#include "../../Tools/inc/Precomputation.h"

GeneticTrainer::GeneticTrainer()
{
	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	Evaluator = std::make_shared<HandEvaluator>();
	ActiveTable = std::make_shared<Table>(Evaluator, 20, false);
	Writer = std::make_unique<LogWriter>();

	//Precomp = std::make_unique<Precomputation>(Evaluator);
}

GeneticTrainer::~GeneticTrainer()
{}

void GeneticTrainer::Initialize()
{
	InitializePopulation(Model.PopulationSize);
	BestPlayer = Population[0];
	WorstPlayer = Population[Population.size() - 1];

	PlayingPopulation.clear();
	PlayingPopulation.reserve(TableSize);
	
	PlayingPopulation.push_back(std::make_unique<Caller>(ActiveTable, 100000));
	PlayingPopulation.push_back(std::make_unique<Raiser>(ActiveTable, 200000));
	PlayingPopulation.push_back(std::make_unique<Randomer>(ActiveTable, 300000));

	PlayingPopulation.push_back(std::make_unique<Folder>(ActiveTable, 400000));
	PlayingPopulation.push_back(std::make_unique<CallRaiser>(ActiveTable, 500000));

	if (!Model.HasHoF)
	{
		PlayingPopulation.push_back(std::make_unique<BlossomPlayer>(ActiveTable, Evaluator, 600000));
		PlayingPopulation.push_back(std::make_unique<BlossomPlayer>(ActiveTable, Evaluator, 700000));
		PlayingPopulation.push_back(std::make_unique<BlossomPlayer>(ActiveTable, Evaluator, 800000));
	}

	Tournaments.clear();
	Tournaments.reserve(Model.TournamentsPerGen);
	for (unsigned int Index = 0; Index < Model.TournamentsPerGen; Index++)
		Tournaments.push_back(std::make_unique<Tournament>(Index, 20, ActiveTable));

	if (Model.HasHoF)
	{
		HoF.clear();
		HoF.reserve(HoFSize);
	}

	#pragma region Logging & Comments
	std::cout << "Simulation starts...\n";

	std::cout << "Population:" << Model.PopulationSize << " GenerationLimit:" << Model.GenerationLimit << "\n";
	std::cout << "Players will play " << Model.TournamentsPerGen << " Tournaments with " << TableSize << " duplicate matches.\n";
	std::cout << "Each generation will crossover at a probability of " << Model.CrossoverRate << " with Elitism (" << ElitesLimit << ") applied and mutate dynamically with delta of " << Model.GaussianOffset << ".\n";

	Writer->NewDir();
	Writer->GenerateGNUFiles();

	Writer->NewFile(LogType::NONE, "Tournament - PopS_" + std::to_string(Model.PopulationSize) + " GenLimit_" + std::to_string(Model.GenerationLimit) + " ToursPerGen_" + std::to_string(Model.TournamentsPerGen));
	Writer->WriteAt(0, "Simulating a Population of " + std::to_string(Model.PopulationSize) + " Players for " + std::to_string(Model.GenerationLimit) + " generations.\n");
	Writer->WriteAt(0, "Players will play " + std::to_string(Model.TournamentsPerGen) + " Tournaments with " + std::to_string(Model.PopulationSize) + " duplicate matches.\n");
	Writer->WriteAt(0, "Each generation will crossover at a probability of " + std::to_string(Model.CrossoverRate) + " with Elitism (" + std::to_string(ElitesLimit) + ") applied and mutate dynamically with delta of " + std::to_string(Model.GaussianOffset) + ".\n");

	Writer->NewFile(LogType::Graph_Line, "GenerationPerformance");
	Writer->NewFile(LogType::Graph_Line, "PopulationVariance");
	Writer->NewFile(LogType::Graph_Line, "MutationRate");
	Writer->NewFile(LogType::Graph_Bar, "HallOfFame");

	Writer->NewFile(LogType::Graph_Line, "BestSoFarFitness");
	Writer->NewFile(LogType::Graph_Line, "WorstSoFarFitness");
	Writer->NewFile(LogType::Graph_Line, "GenEliteAverFitness");
	Writer->NewFile(LogType::Graph_Line, "GenHoFTop3AverFitness");

	Writer->WriteAt(1, "#Generation | Average Fitness\n");
	Writer->WriteAt(2, "#Generation | Variance\n");
	Writer->WriteAt(3, "#Generation | Mutation Rate\n");
	Writer->WriteAt(4, "#Player Index | Fitness\n");

	Writer->WriteAt(5, "#Generation | Best-so-far Fitness\n");
	Writer->WriteAt(6, "#Generation | Worst-so-far Fitness\n");
	Writer->WriteAt(7, "#Generation | Elites Average Fitness\n");
	Writer->WriteAt(8, "#Generation | Top 3 Average Fitness\n");
	#pragma endregion
}

void GeneticTrainer::Run()
{
	Initialize();

	for (unsigned int GenIndex = 0; GenIndex < Model.GenerationLimit; GenIndex++)
	{
		#pragma region Couts & Log Writing
		Writer->WriteAt(0, "\nGeneration " + std::to_string(Generation) + ": (" + std::to_string(PlayersGenerated) + " Players created till now): \n");

		for (auto const& Player : Population)
			Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + GetThresholdsStr(Player) + "\n");

		Writer->WriteAt(0, "\n");
		#pragma endregion

		//Initialize/Refresh Populations
		InitializePlayingPopu();

		/*std::cout << "\nPopulation of " << Population.size() << ":\n";
		for (auto const& Player : Population)
			std::cout << "P." << Player->GetIndex() << " ";
		std::cout << "\n";

		std::cout << "HoF of " << HoF.size() << ":\n";
		for (auto const& Player : HoF)
			std::cout << "P." << Player->GetIndex() << " ";
		std::cout << "\n";*/

		//InitializeEvaluatingPopu();

		/*std::cout << "EvaluatingPopu:\n";
		for (auto const& Player : EvaluatingPopulation)
			std::cout << "P." << Player->GetIndex() << " ";
		std::cout << "\n\n";*/

		//Run tournaments for each player and rank them based on their performance
		/*for (auto& Player : EvaluatingPopulation)
		{
			MeasureFitness(Player);

			//std::cout << "Fitness of P." << Player->GetIndex() << ": " << Player->GetFitness() << "(Average Rank) " << Player->GetRanking() << " (Total Ranking) " << (Model.TournamentsPerGen * TableSize) << " (Total Matches)\n";

			if (Model.FeedbackLayer == Layer::Individual)
				PrintPlayerResult(Player);
		}*/

		for (auto& Player : Population)
		{
			MeasureFitness(Player);

			//std::cout << "Fitness of P." << Player->GetIndex() << ": " << Player->GetFitness() << "(Average Rank) " << Player->GetRanking() << " (Total Ranking) " << (Model.TournamentsPerGen * TableSize) << " (Total Matches)\n";

			if (Model.FeedbackLayer == Layer::Individual)
				PrintPlayerResult(Player);
		}
		
		/*std::cout << "\nPre-arranged Population:\n";
		for (auto const& Player : Population)
			std::cout << "P." << Player->GetIndex() << " - " << Player->GetFitness() << " ";
		std::cout << "\n";

		std::cout << "Pre-arranged HoF:\n";
		for (auto const& Player : HoF)
			std::cout << "P." << Player->GetIndex() << " - " << Player->GetFitness() << " ";
		std::cout << "\n";*/

		RankPlayers();

		//Add the top players into HoF if they perform sufficently well
		if (Model.HasHoF)
		{
			ArrangeHoF();
			AddToHoF(HoFSize);
		}

		/*std::cout << "\nPost-arranged Population:\n";
		for (auto const& Player : Population)
			std::cout << "P." << Player->GetIndex() << " - " << Player->GetFitness() << " ";
		std::cout << "\n";

		std::cout << "Post-arranged HoF:\n";
		for (auto const& Player : HoF)
			std::cout << "P." << Player->GetIndex() << " - " << Player->GetFitness() << " ";
		std::cout << "\n";*/

		if (Model.HasReserveSelection)
		{
			//Measure Uniqueness & Potential of each player
			for (auto const& Player : Population)
			{
				Player->CalculateUniqueness(Population);
				Player->CalculatePotential(Population, Model.SamplingBreadth);
			}

			GenerationAverFitness.push_back(GetOverallFitness());
		}
		
		PrintGenerationResult();

		#pragma region Couts & Log Writing	
		Writer->WriteAt(0, "\nGenerational Ranking: \n");
		for (auto const& Player : Population)
			Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + std::to_string(Player->GetFitness()) + "(Accumluated Rankings: " + std::to_string(Player->GetRanking()) + "/ Games Finished: " + std::to_string(Model.TournamentsPerGen * 9 * (Generation + 1)) + ")\n");
		Writer->WriteAt(0, "\n");

		if (Model.HasHoF)
		{
			Writer->WriteAt(0, "Hall of Fame: \n");
			for (auto const& Player : HoF)
				Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + std::to_string(Player->GetFitness()) + "(Accumluated Rankings: " + std::to_string(Player->GetRanking()) + "/ Games Finished: " + std::to_string(Model.TournamentsPerGen * 9 * (Generation + 1)) + ")\n");
			Writer->WriteAt(0, "\n");

			Writer->WriteAt(8, Generation, (HoF[0]->GetFitness() + HoF[1]->GetFitness() + HoF[2]->GetFitness()) / 3.0f);
		}

		Writer->WriteAt(1, Generation, GetOverallFitness());
		Writer->WriteAt(2, Generation, GetGenerationDiversity());

		Writer->WriteAt(0, "Top Players: P." + std::to_string(Population[0]->GetIndex()) + " (" + std::to_string(Population[0]->GetFitness()) + ") P." + std::to_string(Population[1]->GetIndex()) + " (" + std::to_string(Population[1]->GetFitness()) + ") P." + std::to_string(Population[2]->GetIndex()) + " (" + std::to_string(Population[2]->GetFitness()) + ")\n");
		Writer->WriteAt(0, "Overall Fitness of Generation " + std::to_string(Generation) + ": " + std::to_string(GetOverallFitness()) + "\n");
		Writer->WriteAt(5, Generation, BestPlayer->GetFitness());
		Writer->WriteAt(6, Generation, WorstPlayer->GetFitness());
		Writer->WriteAt(0, "Diversity of Generation " + std::to_string(Generation) + ": " + std::to_string(GetGenerationDiversity()) + "\n");

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

			//std::cout << StdDev << " ";
			Writer->WriteAt(0, std::to_string(StdDev) + " ");
		}
		Writer->WriteAt(0, "\n");

		Writer->WriteAt(3, Generation, ActiveMutator->GetProbability());
		#pragma endregion

		if (Model.HasCulling && !IsTrainingCompleted() && HasPopulationStagnate())
			CullCount < Model.MaxCullCount ? CullPopulation() : NukePopulation();

		else if(!IsTrainingCompleted())
			ReproducePopulation();

		else
			End();
	}/*std::cout << "\nPost-production Population: ";
		for (auto const& Player : Population)
			std::cout << "P." << Player->GetIndex() << " ";
		std::cout << "\n";*/
}

void GeneticTrainer::End()
{
	Writer->CloseAt(0);
	Writer->CloseAt(1);
	Writer->CloseAt(2);
	Writer->CloseAt(3);
	Writer->Clear();
}

void GeneticTrainer::Reset()
{
	Population.clear();

	Generation = 0;
	PlayersGenerated = 0;

	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

bool GeneticTrainer::IsTrainingCompleted()
{
	return (Model.GenerationLimit > 0 && Generation >= (Model.GenerationLimit - 1));
}

void GeneticTrainer::InitializePopulation(unsigned int _Size) 
{
	Population.clear();

	for (unsigned int Index = 0; Index < _Size; Index++)
	{
		Population.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated));
		PlayersGenerated++;
	}
}

void GeneticTrainer::InitializeEvaluatingPopu()
{
	EvaluatingPopulation.clear();
	EvaluatingPopulation.insert(EvaluatingPopulation.end(), Population.begin(), Population.end());
	EvaluatingPopulation.insert(EvaluatingPopulation.end(), HoF.begin(), HoF.end());
}

void GeneticTrainer::InitializePlayingPopu()
{
	std::cout << "\nInitializing PlayingPopu...\n";
	
	std::cout << "Pre-erased PlayingPopu: ";
	for (auto const& Player : PlayingPopulation)
		std::cout << "P." << Player->GetIndex() << " ";
	std::cout << "\n";

	if (Model.HasHoF)
	{
		PlayingPopulation.erase(PlayingPopulation.begin() + 5, PlayingPopulation.end());

		std::cout << "\nPost-erased PlayingPopu: ";
		for (auto const& Player : PlayingPopulation)
			std::cout << "P." << Player->GetIndex() << " ";
		std::cout << "\n";

		unsigned int Index = 800001;

		for (auto const& CurrentPlayer : HoF)
		{
			if (std::find_if(PlayingPopulation.begin(), PlayingPopulation.end(), [&](std::shared_ptr<Player> _ComparedTo) { return _ComparedTo->GetIndex() == CurrentPlayer->GetIndex(); }) == PlayingPopulation.end()
				&& std::find_if(Population.begin(), Population.end(), [&](std::shared_ptr<BlossomPlayer> _ComparedTo) { return _ComparedTo->GetIndex() == CurrentPlayer->GetIndex(); }) == Population.end())
				PlayingPopulation.push_back(std::make_unique<BlossomPlayer>(CurrentPlayer, Index++));

			if (PlayingPopulation.size() >= TableSize - 1)
				break;
		}

		while (PlayingPopulation.size() < TableSize - 1)
			PlayingPopulation.push_back(std::make_unique<BlossomPlayer>(ActiveTable, Evaluator, Index++));

		std::cout << "\nAdded HOFs into PlayingPopu: ";
		for (auto const& Player : PlayingPopulation)
			std::cout << "P." << Player->GetIndex() << " ";
		std::cout << "\n";
	}
	else
	{
		unsigned int Index = 900001;

		while (PlayingPopulation.size() < TableSize - 1)
			PlayingPopulation.push_back(std::make_unique<BlossomPlayer>(ActiveTable, Evaluator, Index++));
	}
}

float GeneticTrainer::MeasureFitness(const std::shared_ptr<BlossomPlayer>& _Player)
{
	if(PlayingPopulation.size() < TableSize - 1)
		return 0.0f;

	_Player->SetInQuestion(true);
	PlayingPopulation.push_back(_Player);
		
	for (auto const& Tournament : Tournaments)
	{
		Tournament->Initialise(PlayingPopulation, PlayingPopulation.size(), Model.HasHoF);
		Tournament->Run();
	}

	_Player->CalculateFitness();

	PlayingPopulation.pop_back();
	_Player->SetInQuestion(false);

	return _Player->GetFitness();
}

void GeneticTrainer::RankPlayers()
{
	std::sort(Population.begin(), Population.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) { return _First->GetFitness() < _Second->GetFitness(); });

	if (Generation == 0)
	{
		BestPlayer = Population[0];
		WorstPlayer = Population[Population.size() - 1];
		return;
	}

	if (Population[0]->GetFitness() < BestPlayer->GetFitness())
		BestPlayer = Population[0];

	else if (Population[Population.size() - 1]->GetFitness() > WorstPlayer->GetFitness())
		WorstPlayer = Population[Population.size() - 1];
}

void GeneticTrainer::AddToHoF(unsigned int _Amt)
{
	if (HoF.size() == 0)
	{
		for (unsigned int Index = 0; Index < _Amt; Index++)
		{
			Population[Index]->SetInHoF(true);
			HoF.push_back(Population[Index]);
		}

		return;
	}

	for (auto const& PopuPlayer : Population)
	{
		for (unsigned int Index = HoF.size() - 1; Index > 0; Index--)
		{
			if (PopuPlayer->GetRanking() < HoF[Index]->GetRanking() && (Index == 0 || PopuPlayer->GetRanking() > HoF[Index - 1]->GetRanking()))
			{
				HoF[Index]->SetInHoF(false);
				PopuPlayer->SetInHoF(true);
				HoF[Index] = PopuPlayer;
				return;
			}
		}
	}

	/*for (unsigned int Index = 0; Index < _Amt; Index++)
	{
		auto TopItr = std::find_if(HoF.begin(), HoF.end(), [&](std::shared_ptr<BlossomPlayer> _Player) { return _Player->GetIndex() == Population[Index]->GetIndex(); });

		if (TopItr == HoF.end())
		{
			Population[Index]->SetInHoF(true);
			HoF.push_back(Population[Index]);

			HoF[HoF.size() - 1]->CalculateFitness();

			Writer->WriteAt(4, std::to_string(Population[Index]->GetIndex()) + " " + std::to_string(Population[Index]->GetFitness()) + "\n");
		}
		/*else if (TopItr != HoF.end())
		{
			float PrevFitness = (*TopItr)->GetFitness();

			(*TopItr)->CalculateFitness();
			
			if ((*TopItr)->GetFitness() != PrevFitness)
				Writer->Overwrite(4, std::to_string(Population[Index]->GetIndex()) + " " + std::to_string(PrevFitness), std::to_string(Population[Index]->GetIndex()) + " " + std::to_string(Population[Index]->GetFitness()));
		}
	}*/
}

void GeneticTrainer::ArrangeHoF()
{
	std::sort(HoF.begin(), HoF.end(),
		[&](std::shared_ptr<BlossomPlayer>& _First, std::shared_ptr<BlossomPlayer>& _Second) { return _First->GetFitness() < _Second->GetFitness(); });
}

void GeneticTrainer::ClipHoF(unsigned int _Size)
{
	if (HoF.size() > _Size)
		HoF.erase(HoF.begin() + _Size, HoF.end());
}

float GeneticTrainer::GetOverallFitness()
{
	float TotalFitness = 0.0;
	for (auto const& Player : Population)
		TotalFitness += Player->GetFitness();

	return TotalFitness / Population.size();
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
	std::uniform_int_distribution<int> Distribution_Qualifier(0, Model.PopulationSize - 1);

	std::vector<std::shared_ptr<BlossomPlayer>> Tournament;
	Tournament.reserve(Model.TournamentSize);

	std::shared_ptr<BlossomPlayer> CurrentPlayer;

	for (unsigned int TourIndex = 0; TourIndex < Model.TournamentSize; TourIndex++)
	{
		do { CurrentPlayer = _RefPopulation[Distribution_Qualifier(MTGenerator)]; } while (std::find(Population.begin(), Population.end(), CurrentPlayer) != Population.end());
		Tournament.push_back(CurrentPlayer);
	}

	std::sort(Tournament.begin(), Tournament.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) {return _First->GetFitness() > _Second->GetFitness(); });
	return Tournament[0];
}

std::shared_ptr<BlossomPlayer> GeneticTrainer::TournamentSelect_Uniqueness(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation)
{
	std::uniform_int_distribution<int> Distribution_Qualifier(0, Model.PopulationSize - 1);

	std::vector<std::shared_ptr<BlossomPlayer>> Tournament;
	Tournament.reserve(Model.TournamentSize);

	std::shared_ptr<BlossomPlayer> CurrentPlayer;

	for (unsigned int TourIndex = 0; TourIndex < Model.TournamentSize; TourIndex++)
	{
		do { CurrentPlayer = _RefPopulation[Distribution_Qualifier(MTGenerator)]; } while (std::find(Population.begin(), Population.end(), CurrentPlayer) != Population.end());
		Tournament.push_back(CurrentPlayer);
	}

	std::sort(Tournament.begin(), Tournament.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) {return _First->GetUniqueness() > _Second->GetUniqueness(); });
	return Tournament[0];
}

std::shared_ptr<BlossomPlayer> GeneticTrainer::TournamentSelect_Potential(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation)
{
	std::uniform_int_distribution<int> Distribution_Qualifier(0, Model.PopulationSize - 1);

	std::vector<std::shared_ptr<BlossomPlayer>> Tournament;
	Tournament.reserve(Model.TournamentSize);

	std::shared_ptr<BlossomPlayer> CurrentPlayer;

	for (unsigned int TourIndex = 0; TourIndex < Model.TournamentSize; TourIndex++)
	{
		do { CurrentPlayer = _RefPopulation[Distribution_Qualifier(MTGenerator)]; } while (std::find(Population.begin(), Population.end(), CurrentPlayer) != Population.end());
		Tournament.push_back(CurrentPlayer);
	}

	std::sort(Tournament.begin(), Tournament.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) {return _First->GetPotential() > _Second->GetPotential(); });
	return Tournament[0];
}

std::shared_ptr<BlossomPlayer> GeneticTrainer::Adapt(const std::shared_ptr<BlossomPlayer>& _Target, const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation)
{
	auto PlayerItr = std::find_if(_RefPopulation.begin(), _RefPopulation.end(), [&](const std::shared_ptr<BlossomPlayer>& _Comparison) { return _Target->GetIndex() == _Comparison->GetIndex(); });
	std::shared_ptr<BlossomPlayer> SuperiorNeighbour = _Target;

	auto MinItr = PlayerItr, MaxItr = PlayerItr;
	for (unsigned int Index = 0; Index < Model.SamplingBreadth; Index++)
	{
		MinItr = std::prev(MinItr,1);
		
		if (MinItr == _RefPopulation.begin())
			break;
	}
	for (unsigned int Index = 0; Index < Model.SamplingBreadth; Index++)
	{
		if (std::next(MaxItr, 1) == _RefPopulation.end())
			break;

		MaxItr = std::next(MaxItr, 1);
	}

	for (auto Itr = MinItr; Itr <= MaxItr; Itr++)
	{
		if (Itr == _RefPopulation.end())
			break;

		if ((*(Itr))->GetFitness() > SuperiorNeighbour->GetFitness())
			SuperiorNeighbour = *(Itr);
	}

	std::shared_ptr<BlossomPlayer> ImprovedPlayer = std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated);
	ImprovedPlayer->GetAI().SetThresholds(SuperiorNeighbour->GetAI().GetThresholds());

	Writer->WriteAt(0, "Newly adapted player P." + std::to_string(ImprovedPlayer->GetIndex()) + " is created based on P." + std::to_string(SuperiorNeighbour->GetIndex()) + "\n");

	PlayersGenerated++;
	return ImprovedPlayer;
}

void GeneticTrainer::ReproducePopulation()
{
	//Writer->WriteAt(0, "\nReproducing population...\n");

	if (Model.IsOverlapping)
	{
		std::vector<std::shared_ptr<BlossomPlayer>> PopulationReference(Population.begin(), Population.end());
		
		/*Writer->WriteAt(0, "Pre-reproduction Population:\n");
		for (auto const& Player : PopulationReference)
			Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + GetThresholdsStr(Player) + "\n");
		std::cout << "\n";*/

		if (Model.HasElite)
			Population.erase(Population.begin() + ElitesLimit, Population.end());
		else
			Population.clear();

		/*std::cout << "\nPre-production Population: ";
		for (auto const& Player : Population)
			std::cout << "P." << Player->GetIndex() << " ";
		std::cout << "\n";*/

		//std::cout << "Reproducing Population...\n";

		while (Population.size() < Model.PopulationSize)
		{
			std::array<std::shared_ptr<BlossomPlayer>, 2> Parents;
			Parents[0] = ActiveSelector->SelectFrom(PopulationReference);
			Parents[1] = ActiveSelector->SelectFrom(PopulationReference);

			/*std::cout << "\nParents:\n";
			std::cout << "P." << Parents[0]->GetIndex() << ": " << GetThresholdsStr(Parents[0]) << "\n";
			std::cout << "P." << Parents[1]->GetIndex() << ": " << GetThresholdsStr(Parents[1]) << "\n";*/

			std::array<std::shared_ptr<BlossomPlayer>, 2> Children;
			Children[0] = std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated);
			PlayersGenerated++;
			while (std::find_if(PlayingPopulation.begin(), PlayingPopulation.end(), [&](std::shared_ptr<Player> _Player) { return _Player->GetIndex() == PlayersGenerated; }) != PlayingPopulation.end())
				PlayersGenerated++;

			Children[1] = std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated);
			PlayersGenerated++;
			while (std::find_if(PlayingPopulation.begin(), PlayingPopulation.end(), [&](std::shared_ptr<Player> _Player) { return _Player->GetIndex() == PlayersGenerated; }) != PlayingPopulation.end())
				PlayersGenerated++;

			//std::cout << "\nChildren:\n";

			ActiveCrossoverer->Cross(Parents, Children);
			
			/*std::cout << "\nPost-crossover: \n";
			std::cout << "P." << Children[0]->GetIndex() << " - " << GetThresholdsStr(Children[0]) << "\n";
			std::cout << "P." << Children[1]->GetIndex() << " - " << GetThresholdsStr(Children[1]) << "\n";*/

			for (auto& Child : Children)
				ActiveMutator->Mutate(Child);

			/*std::cout << "\nPost-mutation: \n";
			std::cout << "P." << Children[0]->GetIndex() << " - " << GetThresholdsStr(Children[0]) << "\n";
			std::cout << "P." << Children[1]->GetIndex() << " - " << GetThresholdsStr(Children[1]) << "\n";*/

			Population.insert(Population.end(), Children.begin(), Children.end());	
		}

		/*std::cout << "\nPost-production Population: ";
		for (auto const& Player : Population)
			std::cout << "P." << Player->GetIndex() << " ";
		std::cout << "\n";*/

		/*Writer->WriteAt(0, "Post-reproduction Population:\n");
		for (auto const& Player : PopulationReference)
			Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + GetThresholdsStr(Player) + "\n");
		std::cout << "\n";*/
	}
	else
	{
		std::vector<std::shared_ptr<BlossomPlayer>> PopulationReference(Population.begin(), Population.end());

		/*Writer->WriteAt(0, "Pre-reproduction Population:\n");
		for (auto const& Player : PopulationReference)
			Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + GetThresholdsStr(Player) + "\n");
		std::cout << "\n";*/

		/*std::cout << "\nPre-production Population: ";
		for (auto const& Player : Population)
		std::cout << "P." << Player->GetIndex() << " ";
		std::cout << "\n";*/

		ActiveSelector->SetMethod(Model.SelectMethod);

		//Reproduce Children based on the population in current generation
		unsigned int ChildPopuSize = Model.PopulationSize * Model.ChildPopulationRatio;

		for (unsigned int Count = 0; Count < ChildPopuSize; Count++)
		{
			std::array<std::shared_ptr<BlossomPlayer>, 2> Parents;
			Parents[0] = ActiveSelector->SelectFrom(PopulationReference);
			Parents[1] = ActiveSelector->SelectFrom(PopulationReference);

			std::array<std::shared_ptr<BlossomPlayer>, 2> Children;
			Children[0] = std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated);
			PlayersGenerated++;
			while (std::find_if(PlayingPopulation.begin(), PlayingPopulation.end(), [&](std::shared_ptr<Player> _Player) { return _Player->GetIndex() == PlayersGenerated; }) != PlayingPopulation.end())
				PlayersGenerated++;

			Children[1] = std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated);
			PlayersGenerated++;
			while (std::find_if(PlayingPopulation.begin(), PlayingPopulation.end(), [&](std::shared_ptr<Player> _Player) { return _Player->GetIndex() == PlayersGenerated; }) != PlayingPopulation.end())
				PlayersGenerated++;

			ActiveCrossoverer->Cross(Parents, Children);

			for (auto& Child : Children)
				ActiveMutator->Mutate(Child);

			Population.insert(Population.end(), Children.begin(), Children.end());
		}

		//Construct PlayingPopulation (Top 5 players from Population Reference + Players from HoF)
		PopulationReference.clear();
		PopulationReference.insert(PopulationReference.end(), Population.begin(), Population.end());

		InitializePlayingPopu();

		for (unsigned int Index = Model.PopulationSize - 1; Index < Model.PopulationSize + ChildPopuSize; Index++)
		{
			MeasureFitness(Population[Index]);
		}

		/*std::cout << "\nNon-overlapped Population: ";
		for (auto const& Player : Population)
			std::cout << "P." << Player->GetIndex() << " ";
		std::cout << "\n";*/

		if (Model.HasElite)
		{
			Population.erase(Population.begin() + ElitesLimit, Population.end());
			PopulationReference.erase(PopulationReference.begin(), PopulationReference.begin() + ElitesLimit);

			/*while (Population.size() < Model.PopulationSize)
				Population.push_back(ActiveSelector->SelectFrom(PopulationReference));*/
		}
			
		else
		{
			Population.clear();

			/*while (Population.size() < Model.PopulationSize)
			{
				Population.push_back(ActiveSelector->SelectFrom(PopulationReference));
			}*/
		}

		while (Population.size() < Model.PopulationSize)
		{
			std::shared_ptr<BlossomPlayer> SelectedPlayer = ActiveSelector->SelectFrom(PopulationReference);

			if (std::find_if(Population.begin(), Population.end(), [&](std::shared_ptr<Player> _ComparedTo) { return _ComparedTo->GetIndex() == SelectedPlayer->GetIndex(); }) != Population.end())
			{
				Population.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated));
				Population[Population.size() - 1]->GetAI().SetThresholds(SelectedPlayer->GetAI().GetThresholds());
				PlayersGenerated++;

				//std::cout << "Duplicated player (P." << SelectedPlayer->GetIndex() << ") was selected.... A new player (P." << Population[Population.size() - 1]->GetIndex() << ") has been added...\n";
			}
			else
			{
				Population.push_back(SelectedPlayer);
			}
		}

		/*bool HasDuplication = false;
		unsigned int FoundCount = 0;
		std::cout << "Check for any duplicated players in Population: ";

		for (auto RefPlayer : Population)
		{
			for (auto SearchedPlayer : Population)
			{
				if (SearchedPlayer->GetIndex() == RefPlayer->GetIndex())
					FoundCount++;

				if (FoundCount >= 2)
				{
					std::cout << "P." << SearchedPlayer->GetIndex() << " ";
					HasDuplication = true;
					break;
				}
			}

			FoundCount = 0;
			if (HasDuplication)
				break;
		}*/

		/*if (HasDuplication)
			std::cout << "FOUND\n";
		else
			std::cout << "NOT FOUND\n";

		std::cout << "\nPost-production Population: ";
		for (auto const& Player : Population)
			std::cout << "P." << Player->GetIndex() << " ";
		std::cout << "\n";*/

		/*Writer->WriteAt(0, "Post-reproduction Population:\n");
		for (auto const& Player : PopulationReference)
			Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + GetThresholdsStr(Player) + "\n");
		std::cout << "\n";*/
	}

	//Reproduction with GARS
	/*std::vector<std::shared_ptr<BlossomPlayer>> PopulationReference(Population.begin(), Population.end());
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
		EliteAverageFitness += Population[Index]->GetFitness();

		//std::cout << "P." << Population[Index]->GetIndex() << " ";
		Writer->WriteAt(0, "P." + std::to_string(Population[Index]->GetIndex()) + " ");
	}
	EliteAverageFitness /= (float)ElitesLimit;
	Writer->WriteAt(7, Generation, EliteAverageFitness);

	//NRA
	//std::cout << "\n\nGenerating Offspring for NRA...\n";
	Writer->WriteAt(0, "\n\nGenerating Offspring for NBA...\n");

	for (unsigned int Index = 0; Index < Model.PopulationSize - ReserveSize - ElitesLimit; Index++)
	{
		if (Population.size() >= (Model.PopulationSize - ReserveSize) || Population.size() >= Model.PopulationSize)
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
				if (Population.size() >= (Model.PopulationSize - ReserveSize) || Population.size() >= Model.PopulationSize)
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
	bool IsAdapting = Distribution_Adaptation(MTGenerator) < Model.AdaptationRate;

	for (unsigned int Index = 0; Index < ReserveSize; Index++)
	{
		if (Population.size() >= Model.PopulationSize)
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

					if (Population.size() >= Model.PopulationSize)
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
	}*/

	Generation++;
}

void GeneticTrainer::CullPopulation()
{
	//Culling players with lowest uniqueness
	/*//Sort RankiingBoard by ascending uniqueness
	std::sort(RankingBoard.begin(), RankingBoard.end(), [&](std::shared_ptr<Participant> _First, std::shared_ptr<Participant> _Second) { return _First->GetUniqueness() < _Second->GetUniqueness(); });

	float CullRatio = 0.25f;
	unsigned int TargetIndex = (unsigned int)(CullRatio * (float)Model.PopulationSize);

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
	//std::sort(RankingBoard.begin(), RankingBoard.end(), [&](std::shared_ptr<Participant> _First, std::shared_ptr<Participant> _Second) { return _First->GetFitness() < _Second->GetFitness(); });

	Writer->WriteAt(0, "Population:\n");
	for (auto const& Player : Population)
		Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + std::to_string(Player->GetFitness()) + "\n");
	Writer->WriteAt(0, "\n\n");

	//Writer->WriteAt(0, "RankingBoard by ascending fitness:\n");
	//for (auto const& Participant : RankingBoard)
	//	Writer->WriteAt(0, "P." + std::to_string(Participant->GetOwner()->GetIndex()) + ": " + std::to_string(Participant->GetFitness()) + "\n");
	//Writer->WriteAt(0, "\n\n");

	float CullRatio = 0.25f;
	unsigned int TargetIndex = (unsigned int)(CullRatio * (float)Model.PopulationSize);

	Writer->WriteAt(0, "CullRatio: " + std::to_string(CullRatio) + " / TargetIndex: " + std::to_string(TargetIndex) + "\n");

	//Erase and replace the lowest fitness players within RankingBoard and Population
	Population.erase(Population.begin() + TargetIndex, Population.end());

	for (unsigned int Index = 0; Index < TargetIndex; Index++)
	{
		Population.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated));
		PlayersGenerated++;
	}

	//Sort RankingBoard by descending fitness
	std::sort(Population.begin(), Population.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) { return _First->GetFitness() > _Second->GetFitness(); });

	Writer->WriteAt(0, "Post-cull Population:\n");
	for (auto const& Player : Population)
		Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + std::to_string(Player->GetFitness()) + "\n");
	Writer->WriteAt(0, "\n\n");

	CullCount++;
	Generation++;
}

void GeneticTrainer::NukePopulation()
{
	Writer->WriteAt(0, "Nuking players and leaving " + std::to_string(ElitesLimit) + " elites in Population...\n");

	Writer->WriteAt(0, "Pre-nuke Population:\n");
	for (auto const& Player : Population)
		Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + std::to_string(Player->GetFitness()) + "\n");
	Writer->WriteAt(0, "\n\n");

	//Erase and replace all players within Population except Elites
	Population.erase(Population.begin() + ElitesLimit, Population.end());

	for (unsigned int Index = 0; Index < Model.PopulationSize - ElitesLimit; Index++)
	{
		Population.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated));
		PlayersGenerated++;
	}
	
	/*for (unsigned int Index = ElitesLimit; Index < Model.PopulationSize; Index++)
	{
		auto Itr = std::find_if(Population.begin(), Population.end(), [&](std::shared_ptr<BlossomPlayer> _Player) { return _Player->GetIndex() == Population[Index]->GetIndex(); });

		if (Itr != Population.end())
		{
			unsigned int ErasedIndex = (*(Itr))->GetIndex();

			Population.erase(Itr);
			Population.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated));

			GetParticipant(ErasedIndex)->Refresh();
			GetParticipant(ErasedIndex)->SetOwner(Population[Population.size() - 1]);

			PlayersGenerated++;
		}
	}*/

	//Sort RankingBoard by descending fitness
	std::sort(Population.begin(), Population.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) { return _First->GetFitness() > _Second->GetFitness(); });

	Writer->WriteAt(0, "Post-nuke Population:\n");
	for (auto const& Player : Population)
		Writer->WriteAt(0, "P." + std::to_string(Player->GetIndex()) + ": " + std::to_string(Player->GetFitness()) + "\n");
	Writer->WriteAt(0, "\n\n");

	CullCount = 0;
	Generation++;
}

bool GeneticTrainer::HasPopulationStagnate()
{
	std::cout << "Current Generation: " << Generation << " / Model.StagnateInterval: " << Model.StagnateInterval << " / Model.StagnatePeriod: " << Model.StagnatePeriod << "\n";

	if (Generation == 0 || Generation % Model.StagnateInterval != 0)
		return false;

	float CurrentFitness = GetOverallFitness();
	float InitialFitness = GenerationAverFitness[Generation - Model.StagnatePeriod];

	Writer->WriteAt(0, "\nChecking for Stagnation (Generation: " + std::to_string(Generation) + " / StaganteInterval: " + std::to_string(Model.StagnateInterval) + " / Model.StagnatePeriod: " + std::to_string(Model.StagnatePeriod) + ")...\n");
	Writer->WriteAt(0, "Generation " + std::to_string(Generation) + "'s Fitness: " + std::to_string(CurrentFitness) + "\n");
	Writer->WriteAt(0, "Reference Generation " + std::to_string(Generation - Model.StagnateInterval) + "'s Fitness: " + std::to_string(InitialFitness) + "\n");
	Writer->WriteAt(0, "Absolute Diff: " + std::to_string(std::abs(CurrentFitness - InitialFitness)) + " / Min Fitness Diff: " + std::to_string(Model.MinimumFitnessDiff) + "\n\n");

	if (std::abs(CurrentFitness - InitialFitness) < Model.MinimumFitnessDiff)
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

void GeneticTrainer::SetSpecs(TrainingModel _Model, Layer _Layer)//unsigned int _PopulationSize, unsigned int _GenerationLimit, unsigned int _ToursPerGen)
{
	Model = _Model;
	FeedbackLayer = _Layer;

	ElitesLimit = (unsigned int)(Model.EliteRatio * (float)Model.PopulationSize);
	HoFSize = (unsigned int)(Model.HoFRatio * (float)Model.PopulationSize);
	ReserveSize = (unsigned int)(Model.ReserveRatio * (float)Model.PopulationSize);

	ActiveSelector = std::make_unique<Selector>(Model.SelectMethod);
	ActiveCrossoverer = std::make_unique<Crossoverer>(Model.CrossMethod, Model.CrossoverRate);
	ActiveMutator = std::make_unique<Mutator>(Model.MutateMethod, Model.MutationRate);
}

void GeneticTrainer::PrintPlayerResult(std::shared_ptr<BlossomPlayer>& _Player)
{
	std::cout << "\nP." << _Player->GetIndex() << ":\n\n";

	std::cout << "Fitness: " << _Player->GetFitness() << "\n";
	std::cout << "Hands Won/Lost: " << _Player->GetHandsWon() << "/" << _Player->GetHandsLost() << "\n";
	std::cout << "Money Won/Lost: " << _Player->GetMoneyWon() << "/" << _Player->GetMoneyLost() << "\n";
}

void GeneticTrainer::PrintGenerationResult()
{
	std::cout << "\nGeneration " << Generation << ":\n\n";
	
	std::cout << "Population size: " << Population.size() << "\n\n";

	std::cout << "PlayingPopulation used (Size of " << PlayingPopulation.size() << "):\n";
	for (auto const& Player : PlayingPopulation)
		std::cout << "P." << Player->GetIndex() << " ";
	std::cout << "\n";

	std::cout << "\nTop 5 Players (HoF Size of " << HoF.size() << "): \n";
	for (unsigned int Index = 0; Index < 5; Index++)
		std::cout << "P." << Population[Index]->GetIndex() << ": " << Population[Index]->GetFitness() << " (Accumluated Rankings: " << std::to_string(Population[Index]->GetRanking()) << "/ Games Finished: " + std::to_string(Model.TournamentsPerGen * 9 * (Generation + 1)) + ")\n";
	std::cout << "\n";
	
	std::cout << "Diversity: " << GetGenerationDiversity() << "\n\n";
	
	std::cout << "Lowest Fitness: " << Population[Population.size() - 1]->GetFitness() << "\n";
	std::cout << "Average Fitness: " << GetOverallFitness() << "\n";
	std::cout << "Highest Fitness: " << Population[0]->GetFitness() << "\n\n";

	std::cout << "Best Player so far: P." << BestPlayer->GetIndex() << " - " << BestPlayer->GetFitness() << "\n";
	std::cout << "Worst Player so far: P." << WorstPlayer->GetIndex() << " - " << WorstPlayer->GetFitness() << "\n";
}