#include "Match.h"

Match::Match(unsigned int _Index, bool _IsDuplicated, const std::vector<std::shared_ptr<Player>>& _Players) 
	: Index(_Index), IsDuplicated(_IsDuplicated)
{
	for (auto const Player : _Players)
		RankingBoard.push_back(std::make_shared<Participant>(Player));
}

Match::~Match()
{}

void Match::RankPlayers()
{
	for (auto const Participant : RankingBoard)
		Participant->UpdateFitness();

	//Sort Participants by Fitness from largest to smallest
	std::sort(RankingBoard.begin(), RankingBoard.end(),
		[](const std::shared_ptr<Participant>& _First, const std::shared_ptr<Participant>& _Second)
		{return _First->GetFitness() > _Second->GetFitness(); });

	for (auto const& Participant : RankingBoard)
	{
		for (unsigned int Rank = 0; Rank < RankingBoard.size(); Rank++)
		{
			if (RankingBoard[Rank]->GetOwner()->GetIndex() == Participant->GetOwner()->GetIndex())
				Participant->SetRank(Participant->GetRank() + 1);
		}
	}
}

void Match::Refresh()
{
	for (auto const Participant : RankingBoard)
		Participant->Refresh();
}

void Match::PrintInfo()
{
	for (auto const& Participant : RankingBoard)
		std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetFitness() << " (Profit per Hand) | " << Participant->GetMoneyWon() << " (MoneyWon) " << Participant->GetMoneyLost() << " (MoneyLost) " << Participant->GetHandsWon()<< " (HandsWon) " << Participant->GetHandsLost() << " (HandsLost)\n";
}