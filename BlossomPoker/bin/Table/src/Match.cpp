#include "../inc/Match.h"

Match::Match(unsigned int _Index, bool _IsDuplicated, const std::vector<std::shared_ptr<Player>>& _Players)
	: Index(_Index), IsDuplicated(_IsDuplicated)
{
	for (auto const _ToBeAdded: _Players)
		RankingBoard.push_back(_ToBeAdded);
}

Match::~Match()
{
}

void Match::RankPlayers()
{
	for (auto const Player : RankingBoard)
		Player->CalculateEarnings();//CalculateFitness();

	//Sort Participants by Fitness from largest to smallest
	std::sort(RankingBoard.begin(), RankingBoard.end(),
		[](const std::shared_ptr<Player>& _First, const std::shared_ptr<Player>& _Second) {return _First->GetEarnings() > _Second->GetEarnings(); });//GetFitness() > _Second->GetFitness(); });

	/*std::cout << "Match Ended:\n";
	for (auto const& Player : RankingBoard)
		std::cout << "P." << Player->GetIndex() << ": " << Player->GetFitness() << " Hands W/L - " << Player->GetHandsWon() << "/" << Player->GetHandsLost() << " Money W/L - " << Player->GetMoneyWon() << "/" << Player->GetMoneyLost() << ")\n";
	std::cout << "\n";*/
}

void Match::Refresh()
{
	for (auto const Player : RankingBoard)
		Player->ClearStats();
}

void Match::Reload(bool _IsDuplicated, const std::vector<std::shared_ptr<Player>>& _Players)
{
	IsDuplicated = _IsDuplicated;

	RankingBoard.clear();
	RankingBoard.reserve(_Players.size());

	for (auto const Player : _Players)
		RankingBoard.push_back(Player);
}

const std::shared_ptr<Player>& Match::GetPlayer(unsigned int _Index)
{
	for (auto const& Player : RankingBoard)
	{
		if (Player->GetIndex() == _Index)
			return Player;
	}

	return RankingBoard[0];
}

void Match::PrintInfo()
{
	for (auto const& Player : RankingBoard)
		std::cout << "P." << Player->GetIndex() << ": " << Player->GetFitness() << " (Hands W/L: " << Player->GetHandsWon() << "/" << Player->GetHandsLost() << " Money W/L: " << Player->GetMoneyWon() << "/" << Player->GetMoneyLost() << ")\n";
}