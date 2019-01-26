#pragma once
#include <vector>
#include <memory>

#include "Participant.h"

class Match
{
public:
	Match(unsigned int _Index, bool _IsDuplicated, const std::vector<std::shared_ptr<Player>>& _Players);
	~Match();

	void RankPlayers();
	void Refresh();
	void Reload(bool _IsDuplicated, const std::vector<std::shared_ptr<Player>>& _Players);

	unsigned int GetIndex() { return Index; }
	bool GetIsDuplicated() { return IsDuplicated; }

	const std::shared_ptr<Participant>& GetParticipant(unsigned int _Index);
	const std::vector<std::shared_ptr<Participant>>& GetRankingBoard() { return RankingBoard; }

	void PrintInfo();

private:
	unsigned int Index = 0;
	bool IsDuplicated = false;
	std::vector<std::shared_ptr<Participant>> RankingBoard;
};