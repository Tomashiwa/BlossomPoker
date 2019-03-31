#pragma once
#include <vector>
#include <memory>

#include "Participant.h"

class Match;
class Table;
class Player;
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
	const std::shared_ptr<Player>& GetPlayer(unsigned int _Index);
	std::vector<std::shared_ptr<Player>> GetRankingBoard() { return RankingBoard; }

private:
	unsigned int Index = 0;
	
	std::shared_ptr<Table> ActiveTable;
	std::vector<std::shared_ptr<Match>> Matches;
	std::vector<std::shared_ptr<Player>> RankingBoard;

	bool IsDuplicated = false;
};

