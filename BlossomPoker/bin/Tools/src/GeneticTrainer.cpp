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

GeneticTrainer::GeneticTrainer()
{
	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	Evaluator = std::make_shared<HandEvaluator>();
	ActiveTable = std::make_shared<Table>(Evaluator, 20, false);
	Writer = std::make_unique<LogWriter>();

	//FoldingPlayer = std::make_unique<Folder>(ActiveTable, 100000);
	CallingPlayer = std::make_unique<Caller>(ActiveTable, 200000);
	RaisingPlayer = std::make_unique<Raiser>(ActiveTable, 300000);
	RandomPlayer = std::make_unique<Randomer>(ActiveTable, 400000);
	RandomPlayer1 = std::make_unique<Randomer>(ActiveTable, 100000);
}

GeneticTrainer::~GeneticTrainer()
{

}

void GeneticTrainer::Start()
{
	GeneratePopulation(PopulationSize);

	PlayingPopulation.clear();
	PlayingPopulation.reserve(TableSize);
	
	PlayingPopulation.push_back(RandomPlayer1);//FoldingPlayer);
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

	HoF.clear();

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
	Writer->NewFile(LogType::Graph_Line, "GenBestFitness");

	Writer->WriteAt(1, "#X Y\n");
	Writer->WriteAt(2, "#X Y\n");
	Writer->WriteAt(3, "#Generation | Mutation Rate\n");
	Writer->WriteAt(4, "#Player Index | Fitness\n");
	Writer->WriteAt(5, "#Generation | Fitness\n");
	#pragma endregion
}

void GeneticTrainer::Run()
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

			for (auto const& Participant : HoF)
			{
				if (std::find_if(PlayingPopulation.begin(), PlayingPopulation.end(), [&](std::shared_ptr<Player> _Player) { return _Player->GetIndex() == Participant->GetOwner()->GetIndex();}) == PlayingPopulation.end() &&
					std::find_if(Population.begin(), Population.end(), [&](std::shared_ptr<BlossomPlayer> _Player) { return _Player->GetIndex() == Participant->GetOwner()->GetIndex(); }) == Population.end())
				{
					PlayingPopulation.push_back(Participant->GetOwner());
				}
				
				if (PlayingPopulation.size() >= TableSize - 1)
					break;
			}
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
			}

			RankPlayer(Player);
			PlayingPopulation.pop_back();
		}
		
		ArrangePlayers(Population);

		for (unsigned int Index = 0; Index < 4; Index++)
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
		Writer->WriteAt(0, "\nGenerational Ranking: \n");
		for (auto const& Participant : RankingBoard)
		{
			std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetFitness() << " (Hands W/L: " << Participant->GetHandsWon() << "/" << Participant->GetHandsLost() << " Money W/L: " << Participant->GetMoneyWon() << "/" << Participant->GetMoneyLost() << ")\n";
			Writer->WriteAt(0, "P." + std::to_string(Participant->GetOwner()->GetIndex()) + ": " + std::to_string(Participant->GetFitness()) + "(Hands W/L: " + std::to_string(Participant->GetHandsWon()) + "/" + std::to_string(Participant->GetHandsLost()) + " Money W/L : " + std::to_string(Participant->GetMoneyWon()) + "/" + std::to_string(Participant->GetMoneyLost()) + ")\n");
		}
		std::cout << "\n";
		Writer->WriteAt(0, "\n");

		std::cout << "HoF: \n";
		Writer->WriteAt(0, "HoF: \n");
		for (auto const& Participant : HoF)
		{
			std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetFitness() << " (Hands W/L: " << Participant->GetHandsWon() << "/" << Participant->GetHandsLost() << " Money W/L: " << Participant->GetMoneyWon() << "/" << Participant->GetMoneyLost() << ")\n";
			Writer->WriteAt(0, "P." + std::to_string(Participant->GetOwner()->GetIndex()) + ": " + std::to_string(Participant->GetFitness()) + "(Hands W/L: " + std::to_string(Participant->GetHandsWon()) + "/" + std::to_string(Participant->GetHandsLost()) + " Money W/L : " + std::to_string(Participant->GetMoneyWon()) + "/" + std::to_string(Participant->GetMoneyLost()) + ")\n");
		}
		std::cout << "\n";
		Writer->WriteAt(0, "\n");

		Writer->WriteAt(1, Generation, GetOverallFitness());
		Writer->WriteAt(2, Generation, GetGenerationDiversity());

		std::cout << "Top Players: P." << Population[0]->GetIndex() << " (" << GetParticipant(Population[0]->GetIndex())->GetFitness() << ") P." << Population[1]->GetIndex() << " (" << GetParticipant(Population[1]->GetIndex())->GetFitness() << ") " << Population[2]->GetIndex() << " (" << GetParticipant(Population[2]->GetIndex())->GetFitness() << ")\n";
		Writer->WriteAt(0, "Top Players: P." + std::to_string(Population[0]->GetIndex()) + " (" + std::to_string(GetParticipant(Population[0]->GetIndex())->GetFitness()) + ") P." + std::to_string(Population[1]->GetIndex()) + " (" + std::to_string(GetParticipant(Population[1]->GetIndex())->GetFitness()) + ") P." + std::to_string(Population[2]->GetIndex()) + " (" + std::to_string(GetParticipant(Population[2]->GetIndex())->GetFitness()) + ")\n");
		std::cout << "Overall Fitness of Generation " << Generation << ": " << GetOverallFitness() << "\n";
		Writer->WriteAt(0, "Overall Fitness of Generation " + std::to_string(Generation) + ": " + std::to_string(GetOverallFitness()) + "\n");
		std::cout << "Best Fitness of Generation " << Generation << ": P." << RankingBoard[0]->GetOwner()->GetIndex() << " - " << RankingBoard[0]->GetFitness() << "\n";
		Writer->WriteAt(5, Generation, RankingBoard[0]->GetFitness());
		std::cout << "Diversity of Generation " << Generation << ": " << GetGenerationDiversity() << "\n";
		Writer->WriteAt(0, "Diversity of Generation " + std::to_string(Generation) + ": " + std::to_string(GetGenerationDiversity()) + "\n");
		
		if (!IsTestComplete())
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

void GeneticTrainer::GeneratePopulation(unsigned int _Size) 
{
	Population.clear();

	for (unsigned int Index = 0; Index < _Size; Index++)
	{
		Population.push_back(std::make_shared<BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated));
		PlayersGenerated++;
	}
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

void GeneticTrainer::ArrangeHoF()
{
	std::sort(HoF.begin(), HoF.end(),
		[&](std::shared_ptr<Participant>& _First, std::shared_ptr<Participant>& _Second) { return _First->GetFitness() > _Second->GetFitness(); });
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

const std::shared_ptr<Player>& GeneticTrainer::GetBestPlayer()
{
	return RankingBoard[0]->GetOwner();
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

std::shared_ptr<BlossomPlayer>& GeneticTrainer::TournamentSelect(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation)//, std::vector<std::shared_ptr<BlossomPlayer>>& _Parents)
{
	std::uniform_int_distribution<int> Distribution_Qualifier(0, PopulationSize - 1);

	std::vector<std::shared_ptr<BlossomPlayer>> Tournament;
	Tournament.reserve(TouramentSize);

	std::shared_ptr<BlossomPlayer> CurrentPlayer;

	for (unsigned int TourIndex = 0; TourIndex < TouramentSize; TourIndex++)
	{
		do { CurrentPlayer = _RefPopulation[Distribution_Qualifier(MTGenerator)]; } while (std::find(Population.begin(), Population.end(), CurrentPlayer) != Population.end());

		Tournament.push_back(CurrentPlayer);
	}

	std::sort(Tournament.begin(), Tournament.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) {return GetParticipant(_First->GetIndex())->GetFitness() > GetParticipant(_Second->GetIndex())->GetFitness(); });

	return Tournament[0];

	/*std::uniform_int_distribution<int> Distribution_Qualifier(0, PopulationSize - 1);

	std::vector<std::shared_ptr<BlossomPlayer>> Tournament;
	Tournament.reserve(TouramentSize);

	std::shared_ptr<BlossomPlayer> CurrentPlayer;

	for (unsigned int ThrIndex = 0; ThrIndex < ParentLimit; ThrIndex++)
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
			ThrIndex--;
	}*/
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

	std::uniform_int_distribution<int> Distribution_Method(0, 1);
	unsigned int MethodIndex = Distribution_Method(MTGenerator);

	if (MethodIndex == 0)
	{
		std::cout << "Crossover: Single-set Binary Crossover\n";
		Writer->WriteAt(0, "Crossover: Single-set Binary Crossover\n");

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
		Writer->WriteAt(0, "Crossover: Cross-set Binary Crossover\n");

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
	}	
}	

//Mutating a Threshold Set
void GeneticTrainer::Mutate(std::shared_ptr<BlossomPlayer>& _Target, Phase _Phase, unsigned int _Index)
{
	//Single Threshold
	std::uniform_real_distribution<float> Distribution_Mutation(-MutateAmt, MutateAmt);
	std::uniform_real_distribution<float> Distribution_Backtrack(0.0, MutateAmt);

	std::array<float, 16> Thresholds = _Target->GetAI().GetThresholds();
	unsigned int TargetIndex = (unsigned int)_Phase + _Index;
	
	Thresholds[TargetIndex] = Thresholds[TargetIndex] + Distribution_Mutation(MTGenerator);
	if (Thresholds[TargetIndex] < 0)
		Thresholds[TargetIndex] = Distribution_Backtrack(MTGenerator);

	//Entire Set
	/*std::uniform_real_distribution<float> Distribution_Mutation(-MutateAmt, MutateAmt);
	std::uniform_real_distribution<float> Distribution_Backtrack(0.0, MutateAmt);

	std::array<float, 4> ThreshSet = _Target->GetAI().GetThresholdsByPhase(_Phase);

	for (auto& Thresh : ThreshSet)
	{
		Thresh = Thresh + Distribution_Mutation(MTGenerator);
		if (Thresh < 0)
			Thresh = Distribution_Backtrack(MTGenerator);
	}

	_Target->GetAI().SetThresholdsByPhase(_Phase, ThreshSet);*/
}

void GeneticTrainer::EvaluateMutateRate()
{
	//Diversity-based Adaptive Mutation
	//float Sensitivity = 0.3f;
	//float TargetDiversity = 0.2f;
	//float CurrentDiversity = GetGenerationDiversity();

	//MutateRate = MutateRate * (1 + (Sensitivity * ((TargetDiversity - CurrentDiversity) / CurrentDiversity)));
	//MutateRate = std::max(0.0f, std::min(MutateRate, 1.0f));

	//Gaussian Distribution
	/*float a = 2.5f, b = 0.5f, c = 0.15f;
	float x = (float) Generation / (float) GenerationLimit;
	float e = std::exp(1.0f);
	MutateRate = pow((a * e), -(pow((x - b), 2) / (2 * pow(c, 2))));*/

	//Oscillating Sine Wave
	float Freq = 48.7f;
	float HeightOffset = 0.0f;//0.5f;
	float GenRatio = (float)Generation / (float)GenerationLimit;
	MutateRate = (sin(Freq * sqrt(GenRatio))) / 2.0f + HeightOffset;

	MutateRate = std::max(0.0f, std::min(MutateRate, 1.0f));

	Writer->WriteAt(3, (float)Generation, MutateRate);
}

//Mutating a specific threshold
/*void GeneticTrainer::Mutate(std::shared_ptr<BlossomPlayer>& _Target, Phase _Phase, unsigned int _ParaIndex)
{
	std::uniform_real_distribution<float> Distribution_Mutation(-MutateAmt, MutateAmt);

	float MutatedThreshold = _Target->GetAI().GetThresholdsByPhase(_Phase)[_ParaIndex] + Distribution_Mutation(MTGenerator);
	
	if (MutatedThreshold < 0)
	{
		std::uniform_real_distribution<float> Distribution_Backtrack(0.0, MutateAmt);
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
	std::cout << "\nReproducing population...\n";
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
		TournamentSelect(PopulationReference, Parents);

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
	std::vector<std::shared_ptr<BlossomPlayer>> PopulationReference(Population.begin(), Population.end());
	Population.erase(Population.begin() + 1, Population.end()); //Keep the best agent

	std::cout << "Elite: P." << Population[0]->GetIndex() << "\n";
	Writer->WriteAt(0, "Elite: P." + std::to_string(Population[0]->GetIndex()) + "\n");

	std::vector<std::shared_ptr<BlossomPlayer>> Parents;
	std::vector<std::shared_ptr<BlossomPlayer>> Children;
	std::shared_ptr<BlossomPlayer> Reference;

	std::uniform_int_distribution<int> Distribution_Phase(0, 3);

	while(Population.size() < PopulationSize)
	{
		Parents.clear();
		Children.clear();

		if (HasCrossoverHappen())
		{
			while (Parents.size() < 2)
			{
				Reference = TournamentSelect(PopulationReference);

				if (std::find_if(Parents.begin(), Parents.end(), [&](std::shared_ptr<BlossomPlayer> _Parent) { return _Parent->GetIndex() == Reference->GetIndex(); }) == Parents.end())
					Parents.push_back(Reference);
			}

			std::cout << "Parents: P." << Parents[0]->GetIndex() << " & P." << Parents[1]->GetIndex() << "...\n";
			Writer->WriteAt(0, "Parents: P." + std::to_string(Parents[0]->GetIndex()) + " & P." + std::to_string(Parents[1]->GetIndex()) + "\n");

			Crossover(Parents[0], Parents[1], Children);

			for (auto& Child : Children)
			{
				Phase TargetPhase = (Phase)Distribution_Phase(MTGenerator);

				for (unsigned int ThrIndex = 0; ThrIndex < 4; ThrIndex++)
				{
					if (HasMutationHappen())
						Mutate(Child, TargetPhase, ThrIndex);
				}
				
				if(Population.size() < PopulationSize)
					Population.push_back(Child);
				
				std::cout << "Crossover: P." << Child->GetIndex() << " (" << GetThresholdsStr(Child) << ")\n";
				Writer->WriteAt(0, "Crossover: P." + std::to_string(Child->GetIndex()) + " (" + GetThresholdsStr(Child) + ")\n");
			}
		}
		else
		{
			Children.push_back(std::move(std::make_shared <BlossomPlayer>(ActiveTable, Evaluator, PlayersGenerated)));
			Reference = TournamentSelect(PopulationReference);

			std::cout << "Reference: P." << Reference->GetIndex() << "\n";
			Writer->WriteAt(0, "Reference: P." + std::to_string(Reference->GetIndex()) + "\n");

			for (unsigned int Index = 0; Index < 4; Index++)
				Children[0]->GetAI().SetThresholdsByPhase((Phase)Index, Reference->GetAI().GetThresholdsByPhase((Phase)Index));

			Phase TargetPhase = (Phase)Distribution_Phase(MTGenerator);

			for (unsigned int ThrIndex = 0; ThrIndex < 4; ThrIndex++)
			{
				if (HasMutationHappen())
					Mutate(Children[0], TargetPhase, ThrIndex);
			}
	
			Population.push_back(Children[0]);
			PlayersGenerated++;

			std::cout << "W/O Crossover: P." << Children[0]->GetIndex() << " (" << GetThresholdsStr(Children[0]) << ")\n";
			Writer->WriteAt(0, "W/O Crossover: P." + std::to_string(Children[0]->GetIndex()) + " (" + GetThresholdsStr(Children[0]) + ")\n");
		}
	}

	for (unsigned int Index = 0; Index < Population.size(); Index++)
	{
		RankingBoard[Index]->Refresh();
		RankingBoard[Index]->SetOwner(Population[Index]);
	}
	
	Generation++;
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
}