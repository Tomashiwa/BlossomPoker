#include "Tournament.h"

#include "Match.h"
#include "Table.h"
#include "HandEvaluator.h"

Tournament::Tournament(unsigned int _BigBlind, const std::shared_ptr<HandEvaluator>& _Evaluator)
{
	ActiveTable = std::make_shared<Table>(_Evaluator, _BigBlind, false);
}

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

	UpdateRankings();
}

void Tournament::Refresh()
{
	for (auto const& Match : Matches)
		Match->Refresh();
}

void Tournament::UpdateRankings()
{
	for (auto const Match : Matches)
	{
		for (unsigned int Index_Participant = 0; Index_Participant < RankingBoard.size(); Index_Participant++)
		{
			RankingBoard[Index_Participant]->SetRank(RankingBoard[Index_Participant]->GetRank() + Match->GetParticipant(Index_Participant)->GetRank());

			RankingBoard[Index_Participant]->SetMoneyWon(RankingBoard[Index_Participant]->GetMoneyWon() + Match->GetParticipant(Index_Participant)->GetMoneyWon());
			RankingBoard[Index_Participant]->SetMoneyLost(RankingBoard[Index_Participant]->GetMoneyLost() + Match->GetParticipant(Index_Participant)->GetMoneyLost());

			RankingBoard[Index_Participant]->SetHandsWon(RankingBoard[Index_Participant]->GetHandsWon() + Match->GetParticipant(Index_Participant)->GetHandsWon());
			RankingBoard[Index_Participant]->SetHandsLost(RankingBoard[Index_Participant]->GetHandsLost() + Match->GetParticipant(Index_Participant)->GetHandsLost());
		}
	}

	for (auto const& Participant : RankingBoard)
	{
		Participant->UpdateFitness();
		Participant->UpdateAverageFitness(Matches.size());
	}
}

void Tournament::PrintRankings()
{
	std::cout << "\nTournament Results:\n";
	for (auto const& Participant : RankingBoard)
		std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetAverageFitness() << " (Hands Won: " << Participant->GetHandsWon() << ", Hands Lost: " << Participant->GetHandsLost() << ", Money Won: " << Participant->GetMoneyWon() << ", Money Lost: " << Participant->GetMoneyLost() << ")\n";
	std::cout << "\n";
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
	for (auto const Participant : RankingBoard)
	{
		if (Participant->GetOwner()->GetIndex() == _Player->GetIndex())
			return Participant->GetAverageFitness();
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