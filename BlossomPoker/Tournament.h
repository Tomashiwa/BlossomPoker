#pragma once
#include <vector>
#include <memory>

#include "Participant.h"
#include "Player.h"

//class Player;
class Table;
class HandEvaluator;

struct Match
{
	unsigned int Index;
	bool IsDuplicated = false;
	std::vector<std::shared_ptr<Participant>> RankingBoard;

	Match(unsigned int _Index, bool _IsDuplicated, const std::vector<std::shared_ptr<Player>>& _Players, const std::shared_ptr<HandEvaluator>& _Evaluator) : Index(_Index), IsDuplicated(_IsDuplicated)
	{
		for (auto const Player : _Players)
			RankingBoard.push_back(std::make_shared<Participant>(Player, _Evaluator));
	}

	void RankPlayers()
	{
		//std::cout << "Fitness:\n";
		for (auto const Participant : RankingBoard)
		{
			Participant->UpdateFitness();
			//std::cout << "P." << Participant->Owner->GetIndex() << ": " << Participant->Fitness << " ";
		}

		//Sort Participants by Fitness from largest to smallest
		std::sort(RankingBoard.begin(), RankingBoard.end(),
			[](const std::shared_ptr<Participant>& _First, const std::shared_ptr<Participant>& _Second)
			{return _First->Fitness > _Second->Fitness; });

		for (auto const Participant : RankingBoard)
			Participant->Rank = GetRank(Participant->Owner);
	}

	unsigned int GetRank(const std::shared_ptr<Player>& _Player)
	{
		for (unsigned int Rank = 0; Rank < RankingBoard.size(); Rank++)
		{
			if (RankingBoard[Rank]->Owner->GetIndex() == _Player->GetIndex())
				return Rank + 1;
		}

		return 0;
	}

	float GetFitness(const std::shared_ptr<Player>& _Player)
	{
		for (auto const& Participant : RankingBoard)
		{
			if (Participant->Owner->GetIndex() == _Player->GetIndex())
			{
				//Participant->UpdateAverageFitness;
				return Participant->AverageFitness;
			}
		}

		return 0.0;
	}

	void GetParticipant(const std::shared_ptr<Player>& _Player, std::shared_ptr<Participant>& _Participant)
	{
		for (auto const& Participant : RankingBoard)
		{
			if (Participant->Owner->GetIndex() == _Player->GetIndex())
			{
				_Participant = Participant;
				return;
			}
		}
	}

	void PrintInfo()
	{
		for (auto const& Pariticipant : RankingBoard)
			std::cout << "P." << Pariticipant->Owner->GetIndex() << ": " << Pariticipant->MoneyWon << " (MoneyWon) " << Pariticipant->MoneyLost << " (MoneyLost) " << (Pariticipant->HandsWon + Pariticipant->HandsLost) << " (Hands Played) " << Pariticipant->HandsWon << " (HandsWon) " << Pariticipant->HandsLost << " (HandsLost)\n";
	}
};

class Tournament
{
public:
	Tournament(unsigned int _BigBlind);
	~Tournament();

	void Initialise(const std::vector<std::shared_ptr<Player>>& _Players, unsigned int _Size, bool _IsDuplicated);
	void Run();
	
	void UpdateRankings();

	void PrintRankings();
	
	void GetBestPlayer(std::shared_ptr<Player>& _BestPlayer);
	void GetBestParticipant(std::shared_ptr<Participant>& _BestParti);
	void GetArrangedPlayers(std::vector<std::shared_ptr<Player>>& _ArrangedPlayers);

	float GetAverageFitness(const std::shared_ptr<Player>& _Player);
	unsigned int GetAverageRank(const std::shared_ptr<Player>& _Player);

	std::shared_ptr<Table> GetTable() { return ActiveTable; }
	std::vector<std::shared_ptr<Participant>> GetRankingBoard() { return RankingBoard; }

private:
	std::shared_ptr<HandEvaluator> Evaluator;
	std::shared_ptr<Table> ActiveTable;
	std::vector<std::shared_ptr<Match>> Matches;
	std::vector<std::shared_ptr<Participant>> RankingBoard;

	bool IsDuplicated = false;
};

