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
		Matches.push_back(std::make_shared<Match>(Index, IsDuplicated, _Players));

	RankingBoard.clear();
	RankingBoard.reserve(_Players.size());
	for (auto const Player : _Players)
		RankingBoard.push_back(std::make_shared<Participant>(Player));
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
		ActiveTable->Run();

		Match->PrintInfo();

		Match->RankPlayers();

		std::cout << "Rankings:\n";
		for (auto const Participant : Match->RankingBoard)
			std::cout << "P." << Participant->Owner->GetIndex() << " ";
		std::cout << "\n";

		ActiveTable->Reset(false);
		ActiveTable->ShiftDealer(ActiveTable->GetNextPlayer(ActiveTable->GetFirstPlayer()));
	}

	UpdateRankings();
}

void Tournament::UpdateRankings()
{
	for (auto const Match : Matches)
	{
		for (unsigned int Index_Player = 0; Index_Player < RankingBoard.size(); Index_Player++)
		{
			RankingBoard[Index_Player]->Rank += Match->RankingBoard[Index_Player]->Rank;

			RankingBoard[Index_Player]->MoneyWon += Match->RankingBoard[Index_Player]->MoneyWon;
			RankingBoard[Index_Player]->MoneyLost += Match->RankingBoard[Index_Player]->MoneyLost;

			RankingBoard[Index_Player]->HandsWon += Match->RankingBoard[Index_Player]->HandsWon;
			RankingBoard[Index_Player]->HandsLost += Match->RankingBoard[Index_Player]->HandsLost;
		}
	}

	for (auto const& Player : RankingBoard)
		std::cout << "P." << Player->Owner->GetIndex() << ": " << Player->MoneyWon << " (MoneyWon) " << Player->MoneyLost << " (MoneyLost) " << (Player->HandsWon + Player->HandsLost) << " (Hands Played) " << Player->HandsWon << " (HandsWon) " << Player->HandsLost << " (HandsLost)\n";

	for (auto const Participant : RankingBoard)
	{
		Participant->Rank /= (unsigned int) Matches.size();

		Participant->MoneyWon /= (unsigned int) Matches.size();
		Participant->MoneyLost /= (unsigned int) Matches.size();
		Participant->HandsWon /= (unsigned int) Matches.size();
		Participant->HandsLost /= (unsigned int) Matches.size();
	
		Participant->UpdateFitness();
	}

	std::cout << "\nFitness: \n";
	for (auto const& Player : RankingBoard)
		std::cout << "P." << Player->Owner->GetIndex() << ": " << Player->Fitness << " ";
	std::cout << "\n";
}

void Tournament::PrintRankings()
{
	std::cout << "Tournament Rankings:\n";
	for (auto const& Participant : RankingBoard)
		std::cout << "P." << Participant->Owner->GetIndex() << ": " << Participant->Fitness << " (Hands Won: " << Participant->HandsWon << ", Hands Lost: " << Participant->HandsLost << ", Money Won: " << Participant->MoneyWon << ", Money Lost: " << Participant->MoneyLost << ")\n";
}

void Tournament::GetBestPlayer(std::shared_ptr<Player>& _BestPlayer)
{
	_BestPlayer = RankingBoard[0]->Owner;
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
			return Participant->Fitness;
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