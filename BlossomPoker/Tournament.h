#pragma once
#include <vector>
#include <memory>

#include "Participant.h"
#include "Player.h"

class Match;
class Table;
class HandEvaluator;

class Tournament
{
public:
	Tournament(unsigned int _Index, unsigned int _BigBlind, const std::shared_ptr<Table>& _Table);
	~Tournament();

	void Initialise(const std::vector<std::shared_ptr<Player>>& _Players, unsigned int _Size, bool _IsDuplicated);
	void Run();
	void Refresh();

	void RankPlayers();

	void PrintRankings();

	unsigned int GetIndex() { return Index; }

	const std::shared_ptr<Participant>& GetParticipant(unsigned int _Index);
	void GetBestPlayer(std::shared_ptr<Player>& _BestPlayer);
	void GetBestParticipant(std::shared_ptr<Participant>& _BestParti);
	void GetArrangedPlayers(std::vector<std::shared_ptr<Player>>& _ArrangedPlayers);

	float GetAverageFitness(const std::shared_ptr<Player>& _Player);
	unsigned int GetAverageRank(const std::shared_ptr<Player>& _Player);

	std::vector<std::shared_ptr<Participant>> GetRankingBoard() { return RankingBoard; }

private:
	unsigned int Index = 0;
	
	std::shared_ptr<Table> ActiveTable;
	std::vector<std::shared_ptr<Match>> Matches;
	std::vector<std::shared_ptr<Participant>> RankingBoard;

	bool IsDuplicated = false;
};

