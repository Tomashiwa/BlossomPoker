#include "../inc/Tournament.h"

#include "../../Table/inc/Match.h"
#include "../../Table/inc/Table.h"
#include "../../Player/inc/Player.h"
#include "../../Cards/inc/HandEvaluator.h"

Tournament::Tournament(unsigned int _Index, unsigned int _BigBlind, const std::shared_ptr<Table>& _Table)
	: Index(_Index), ActiveTable(_Table)
{}

Tournament::~Tournament()
{}

void Tournament::Initialise(const std::vector<std::shared_ptr<Player>>& _Players, unsigned int _Size, bool _IsDuplicated)
{
	IsDuplicated = _IsDuplicated;

	//Wipe Template Players' statistics that may remain from previous tournaments
	for (unsigned int Index = 0; Index < _Players.size() - 1; Index++)
		_Players[Index]->ClearStats();

	if (Matches.size() != _Size)
	{
		Matches.clear();
		Matches.reserve(_Size);
		for (unsigned int Index = 0; Index < _Size; Index++)
			Matches.push_back(std::make_shared<Match>(Index, IsDuplicated, _Players));
	}
	else
	{
		for (unsigned int Index = 0; Index < _Size; Index++)
			Matches[Index]->Reload(_IsDuplicated, _Players);
	}

	RankingBoard.clear();
	RankingBoard.reserve(_Players.size());

	for (auto const ToBeAdded : _Players)
		RankingBoard.push_back(ToBeAdded);
}

void Tournament::Run()
{
	ActiveTable->Reset(true);
	for (auto const Player : Matches[0]->GetRankingBoard())
		ActiveTable->AddPlayer(Player);

	for (auto const Match : Matches)
	{
		ActiveTable->SetMatch(Match);

		if (!Match->GetIsDuplicated() || (Match->GetIsDuplicated() && Match->GetIndex() == 0))
			ActiveTable->RestockDeck();

		ActiveTable->Run(true);

		Match->RankPlayers();

		ActiveTable->Reset(false);
		ActiveTable->ShiftDealer(ActiveTable->GetNextPlayer(ActiveTable->GetFirstPlayer()));

		if (Match->GetIsDuplicated())
		{
			if (Match->GetIndex() == 0)
				ActiveTable->SaveDeckArrangement();
			else
				ActiveTable->LoadDeckArrangement();
		}
	}

	RankPlayers();
}

void Tournament::Refresh()
{
	for (auto const& Match : Matches)
		Match->Refresh();
}

void Tournament::RankPlayers()
{
	//Average out Players' stats as the matches are duplicates
	for (auto const& Player : RankingBoard)
		Player->CalculateFitness();

	//Sort the RankingBoard's participant from highest profit to lowest profit
	std::sort(RankingBoard.begin(), RankingBoard.end(), [](const std::shared_ptr<Player>& _First, const std::shared_ptr<Player>& _Second) {return _First->GetFitness() > _Second->GetFitness(); });

	//std::cout << "Tournament Ended:\n";
	//for (auto const& Player : RankingBoard)
	//	std::cout << "P." << Player->GetIndex() << ": " << Player->GetFitness() << " Hands W/L - " << Player->GetHandsWon() << "/" << Player->GetHandsLost() << " Money W/L - " << Player->GetMoneyWon() << "/" << Player->GetMoneyLost() << ")\n";
	//std::cout << "\n";
}

void Tournament::PrintRankings()
{
	std::cout << "\nResults:\n";
	for (auto const& Player : RankingBoard)
		std::cout << "P." << Player->GetIndex() << ": " << Player->GetFitness() << " (Hands W/L: " << Player->GetHandsWon() << "/" << Player->GetHandsLost() << " Money W/L: " << Player->GetMoneyWon() << "/" << Player->GetMoneyLost() << ")\n";
	std::cout << "\n";
}

const std::shared_ptr<Player>& Tournament::GetPlayer(unsigned int _Index)
{
	for (auto const& Player : RankingBoard)
	{
		if (Player->GetIndex() == _Index)
			return Player;
	}

	return RankingBoard[0];
}