#include "Tournament.h"

//#include "Player.h"
#include "Table.h"
#include "HandEvaluator.h"

Tournament::Tournament(unsigned int _BigBlind)
{
	Evaluator = std::make_shared<HandEvaluator>();
	ActiveTable = std::make_shared<Table>(Evaluator, _BigBlind, false);
}

Tournament::~Tournament()
{}

void Tournament::Initialise(const std::vector<std::shared_ptr<Player>>& _Players, unsigned int _Size, bool _IsDuplicated)
{
	IsDuplicated = _IsDuplicated;

	Matches.clear();
	Matches.reserve(_Size);
	for (unsigned int Index = 0; Index < _Size; Index++)
		Matches.push_back(std::make_shared<Match>(Index, IsDuplicated, _Players, Evaluator));

	RankingBoard.clear();
	RankingBoard.reserve(_Players.size());
	for (auto const Player : _Players)
		RankingBoard.push_back(std::make_shared<Participant>(Player, Evaluator));
}

void Tournament::Run()
{
	ActiveTable->Reset(true);
	for (auto const Participant : Matches[0]->RankingBoard)
		ActiveTable->AddPlayer(Participant->Owner);

	for (auto const Match : Matches)
	{
		std::cout << "\nRunning Match #" << Match->Index << "...		\n";// \r";

		ActiveTable->SetMatch(Match);

		if (!Match->IsDuplicated || (Match->IsDuplicated && Match->Index == 0))
			ActiveTable->RestockDeck();

		ActiveTable->Run();

		Match->PrintInfo();

		Match->RankPlayers();

		std::cout << "Rankings:\n";
		for (auto const Participant : Match->RankingBoard)
			std::cout << "P." << Participant->Owner->GetIndex() << " ";
		std::cout << "\n";

		ActiveTable->Reset(false);
		ActiveTable->ShiftDealer(ActiveTable->GetNextPlayer(ActiveTable->GetFirstPlayer()));
	
		if (Match->IsDuplicated)
		{
			if (Match->Index == 0)
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
			RankingBoard[Index_Participant]->Rank += Match->RankingBoard[Index_Participant]->Rank;

			RankingBoard[Index_Participant]->MoneyWon += Match->RankingBoard[Index_Participant]->MoneyWon;
			RankingBoard[Index_Participant]->MoneyLost += Match->RankingBoard[Index_Participant]->MoneyLost;

			RankingBoard[Index_Participant]->HandsWon += Match->RankingBoard[Index_Participant]->HandsWon;
			RankingBoard[Index_Participant]->HandsLost += Match->RankingBoard[Index_Participant]->HandsLost;
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
		std::cout << "P." << Participant->Owner->GetIndex() << ": " << Participant->AverageFitness << " (Hands Won: " << Participant->HandsWon << ", Hands Lost: " << Participant->HandsLost << ", Money Won: " << Participant->MoneyWon << ", Money Lost: " << Participant->MoneyLost << ")\n";
	std::cout << "\n";
}

void Tournament::GetBestPlayer(std::shared_ptr<Player>& _BestPlayer)
{
	_BestPlayer = RankingBoard[0]->Owner;
}

void Tournament::GetBestParticipant(std::shared_ptr<Participant>& _BestParti)
{
	_BestParti = RankingBoard[0];
}

void Tournament::GetArrangedPlayers(std::vector<std::shared_ptr<Player>>& _ArrangedPlayers)
{
	_ArrangedPlayers.clear();

	for (auto const Participant : RankingBoard)
		_ArrangedPlayers.push_back(Participant->Owner);
}

float Tournament::GetAverageFitness(const std::shared_ptr<Player>& _Player)
{
	for (auto const Participant : RankingBoard)
	{
		if (Participant->Owner->GetIndex() == _Player->GetIndex())
			return Participant->AverageFitness;
	}

	return 0.0;
}

unsigned int Tournament::GetAverageRank(const std::shared_ptr<Player>& _Player)
{
	for (auto const Participant : RankingBoard)
	{
		if (Participant->Owner->GetIndex() == _Player->GetIndex())
			return Participant->Rank;
	}

	return 0;
}