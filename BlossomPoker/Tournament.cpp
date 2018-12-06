#include "Tournament.h"

#include "Match.h"
#include "Table.h"
#include "HandEvaluator.h"

Tournament::Tournament(unsigned int _Index, unsigned int _BigBlind, const std::shared_ptr<Table>& _Table) 
	: Index(_Index), ActiveTable(_Table)
{}

Tournament::~Tournament()
{}

void Tournament::Initialise(const std::vector<std::shared_ptr<Player>>& _Players, unsigned int _Size, bool _IsDuplicated)
{
	IsDuplicated = _IsDuplicated;

	Matches.clear();
	Matches.reserve(_Size);
	for (unsigned int Index = 0; Index < _Size; Index++)
		Matches.push_back(std::make_shared<Match>(Index, IsDuplicated, _Players));

	RankingBoard.clear();
	RankingBoard.reserve(_Players.size());
	for (auto const Player : _Players)
		RankingBoard.push_back(std::make_shared<Participant>(Player));
}

void Tournament::Run()
{
	ActiveTable->Reset(true);
	for (auto const Participant : Matches[0]->GetRankingBoard())
		ActiveTable->AddPlayer(Participant->GetOwner());

	for (auto const Match : Matches)
	{
		std::cout << "\nRunning Match #" << Match->GetIndex() << "...		\n";// \r";

		ActiveTable->SetMatch(Match);

		if (!Match->GetIsDuplicated() || (Match->GetIsDuplicated() && Match->GetIndex() == 0))
			ActiveTable->RestockDeck();

		ActiveTable->Run();

		Match->RankPlayers();
		Match->PrintInfo();

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
	for (auto const& Match : Matches)
	{
		for (auto const& Participant : RankingBoard)
		{
			//Participant->SetRank(Participant->GetRank() + Match->GetParticipant(Participant->GetOwner()->GetIndex())->GetRank());
		
			Participant->SetMoneyWon(Participant->GetMoneyWon() + Match->GetParticipant(Participant->GetOwner()->GetIndex())->GetMoneyWon());
			Participant->SetMoneyLost(Participant->GetMoneyLost() + Match->GetParticipant(Participant->GetOwner()->GetIndex())->GetMoneyLost());

			Participant->SetHandsWon(Participant->GetHandsWon() + Match->GetParticipant(Participant->GetOwner()->GetIndex())->GetHandsWon());
			Participant->SetHandsLost(Participant->GetHandsLost() + Match->GetParticipant(Participant->GetOwner()->GetIndex())->GetHandsLost());
		}
	}

	//Average out Players' stats as the matches are duplicates
	for (auto const& Participant : RankingBoard)
	{
		//Participant->SetMoneyWon(Participant->GetMoneyWon() / (float) Matches.size());
		//Participant->SetMoneyLost(Participant->GetMoneyLost() / (float)Matches.size());

		//Participant->SetHandsWon(Participant->GetHandsWon() / (float) Matches.size());
		//Participant->SetHandsLost(Participant->GetHandsLost() / (float) Matches.size());

		Participant->UpdateFitness();
	}

	//Sort the RankingBoard's participant from highest profit to lowest profit
	std::sort(RankingBoard.begin(), RankingBoard.end(),
		[](const std::shared_ptr<Participant>& _First, const std::shared_ptr<Participant>& _Second)
		{return _First->GetFitness() > _Second->GetFitness(); });

	//Re-allocate player's rank based on their average stats
	for (auto const& Participant : RankingBoard)
	{
		for (unsigned int Rank = 0; Rank < RankingBoard.size(); Rank++)
		{
			if (RankingBoard[Rank]->GetOwner()->GetIndex() == Participant->GetOwner()->GetIndex())
				Participant->SetRank(Rank + 1);
		}
	}
}

void Tournament::PrintRankings()
{
	std::cout << "\nResults:\n";
	for (auto const& Participant : RankingBoard)
		std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetFitness() << " (MoneyWon: " << Participant->GetMoneyWon() << ", HandsWon: " << Participant->GetHandsWon() << ", MoneyLost: " << Participant->GetMoneyLost() << ", HandsLost: " << Participant->GetHandsLost() << ")\n";
	std::cout << "\n";
}

const std::shared_ptr<Participant>& Tournament::GetParticipant(unsigned int _Index)
{
	for (auto const& Participant : RankingBoard)
	{
		if (Participant->GetOwner()->GetIndex() == _Index)
			return Participant;
	}

	return RankingBoard[0];
}

void Tournament::GetBestPlayer(std::shared_ptr<Player>& _BestPlayer)
{
	_BestPlayer = RankingBoard[0]->GetOwner();
}

void Tournament::GetBestParticipant(std::shared_ptr<Participant>& _BestParti)
{
	_BestParti = RankingBoard[0];
}

void Tournament::GetArrangedPlayers(std::vector<std::shared_ptr<Player>>& _ArrangedPlayers)
{
	_ArrangedPlayers.clear();

	for (auto const Participant : RankingBoard)
		_ArrangedPlayers.push_back(Participant->GetOwner());
}

float Tournament::GetAverageFitness(const std::shared_ptr<Player>& _Player)
{
	for (auto const& Participant : RankingBoard)
	{
		if (Participant->GetOwner()->GetIndex() == _Player->GetIndex())
			return Participant->GetFitness();
	}

	return 0.0;
}

unsigned int Tournament::GetAverageRank(const std::shared_ptr<Player>& _Player)
{
	for (auto const Participant : RankingBoard)
	{
		if (Participant->GetOwner()->GetIndex() == _Player->GetIndex())
			return Participant->GetRank();
	}

	return 0;
}