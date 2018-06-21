#include "Board.h"
#include "GameManager.h"
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "HandEvaluator.h"

#include <algorithm>
#include <iterator>

Board::Board(GameManager* _Manager, unsigned int _BigBlind)
{
	Manager = _Manager;
	Evaluator = _Manager->GetEvaluator();

	Pot = 0;
	SmallBlind = _BigBlind/2;
	BigBlind = _BigBlind;
	RequiredAnte = BigBlind;
	EntryStack = 100 * BigBlind;

	PlayingDeck = new Deck();
}

Board::~Board()
{
}

void Board::InitEvaluator()
{
	memset(HR, 0, sizeof(HR));
	FILE * fin = fopen("HANDRANKS.DAT", "rb");

	// Load the HANDRANKS.DAT file data into the HR array
	size_t bytesread = fread(HR, sizeof(HR), 1, fin);
	printf("read %zu bytes\n", bytesread * sizeof(*HR));
	fclose(fin);

}

int Board::GetHandValue(int* _Cards)
{
	int *p1 = HR + HR[53 + _Cards[0]];
	int *p2 = HR + p1[_Cards[1]];
	int *p3 = HR + p2[_Cards[2]];
	int *p4 = HR + p3[_Cards[3]];
	int *p5 = HR + p4[_Cards[4]];

	return p5[0];
}

void Board::Test()
{
	InitEvaluator();

	/*Rank Range : 2...A(Integer Representation : 0...12)
	Suit Range : C, D, H, S(Integer Representaton : 0..3)
	Card Integer Representation = Rank.IR * 4 + Suit.IR + 1*/
	int Hand1[] = { 4, 10, 17, 25, 35 };
	int Hand2[] = { 38, 49, 7, 19, 23 };

	int HV1 = GetHandValue(Hand1);
	int HV2 = GetHandValue(Hand2);

	std::cout << "Hand 1's Value: " << HV1 << " / Catagory: " << (HV1 >> 12) << std::endl;
	std::cout << "Hand 2's Value: " << HV2 << " / Catagory: " << (HV2 >> 12) << std::endl;
}

void Board::Start()
{
	//Test();
	StartRound();
}

void Board::Update()
{
	if (IsActive)
		UpdateRound();
}

void Board::End()
{
	Player* Winner = Players[0];

	for (unsigned int Index = 0; Index < Players.size(); Index++)
		if (!Players[Index]->GetIsBroke()) Winner = Players[Index];

	std::cout << "This Board has been won by Player " << Winner->GetIndex() << " with $" << Winner->GetStack() << std::endl;
}

void Board::StartRound()
{
	CurrentState = Phase::Preflop;

	for (unsigned int Index = 0; Index < Players.size(); Index++)
	{
		if (!Players[Index]->GetIsBroke())
			Players[Index]->SetIsParticipating(true);

		Players[Index]->EmptyPotContributon();
	}

	DealingPlayer = DealingPlayer == nullptr ? Players[0] : GetNextPlayer(DealingPlayer);
	SmallBlindPlayer = GetNextPlayer(DealingPlayer);
	BigBlindPlayer = GetNextPlayer(SmallBlindPlayer);

	std::cout << "A round has started..." << std::endl;
	std::cout << "DEALING: Player " << DealingPlayer->GetIndex() << " / SBLIND: Player " << SmallBlindPlayer->GetIndex() << " / BBLIND: Player " << BigBlindPlayer->GetIndex() << std::endl;
	std::cout << "========================================" << std::endl;

	StartPhase();
}

void Board::UpdateRound()
{
	UpdatePhase();
}

void Board::EndRound()
{
	std::cout << "========================================" << std::endl;
	std::cout << "A round has ended..." << std::endl;

	std::vector<unsigned int> Pots;
	std::vector<std::vector<Player*>> ValidPlayersPerPot;

	SplitPot(Pots, ValidPlayersPerPot);

	for (unsigned int PotIndex = 0; PotIndex < Pots.size(); PotIndex++)
	{
		std::cout << "Pot: $" << Pots[PotIndex] << " / Participants: ";
		for (int Index = 0; Index < ValidPlayersPerPot[PotIndex].size(); Index++)
			std::cout << "Player " << ValidPlayersPerPot[PotIndex][Index]->GetIndex() << " ($" << ValidPlayersPerPot[PotIndex][Index]->GetPotContribution() << ")  ";
		std::cout << std::endl;
		
		if (ValidPlayersPerPot[PotIndex].size() == 1)
		{
			AwardPlayer(ValidPlayersPerPot[PotIndex][0], Pots[PotIndex]);
			continue;
		}

		std::vector<Player*> PotWinners = DetermineWinningPlayers(ValidPlayersPerPot[PotIndex]);

		if (PotWinners.size() == 1)
		{
			AwardPlayer(PotWinners[0], Pots[PotIndex]);
			std::cout << "Player " << PotWinners[0]->GetIndex() << " win $" << Pots[PotIndex] << " (Stack: $" << PotWinners[0]->GetStack() << ")" << std::endl;
		}

		else if (PotWinners.size() > 1)
		{
			unsigned int Portion = Pots[PotIndex] / PotWinners.size();

			for (unsigned int WinnerIndex = 0; WinnerIndex < PotWinners.size(); WinnerIndex++)
			{
				AwardPlayer(PotWinners[WinnerIndex], Portion);
				std::cout << "Player " << PotWinners[WinnerIndex]->GetIndex() << " win $" << Portion << " (Stack: $" << PotWinners[WinnerIndex]->GetStack() << ")" << std::endl;
			}
		}
	}

	ClearCommunalCards();
	EmptyPot();

	for (unsigned int Index = 0; Index < Players.size(); Index++)
	{
		Players[Index]->EmptyHand();
		Players[Index]->SetAnte(0);

		if (Players[Index]->GetStack() <= 0)
		{
			Players[Index]->SetIsBroke(true);
			std::cout << "Player " << Index << " is broke..." << std::endl;
		}
	}

	std::cout << "========================================" << std::endl;

	if (IsGameEnded())
	{
		IsActive = false;
		return;
	}

	StartRound();
}

void Board::StartPhase()
{
	for (unsigned int Index = 0; Index < Players.size(); Index++)
	{
		Players[Index]->SetAction(BettingAction::NONE);
		Players[Index]->SetAnte(0);
	}

	RequiredAnte = 0;

	switch (CurrentState)
	{
		case Phase::Preflop:
		{
			std::cout << "Current Phase: Pre-flop" << std::endl;
			
			SmallBlindPlayer->SetAnte(SmallBlind);
			BigBlindPlayer->SetAnte(BigBlind);
			CurrentPlayer = GetNextPlayer(BigBlindPlayer);

			RequiredAnte = BigBlind;
			UpdatePot();

			DealCardsToPlayers();
			std::cout << "Hole cards are dealt to players..." << std::endl;			
			break;
		}
		case Phase::Flop:
		{
			std::cout << "Current Phase: Flop" << std::endl;

			CurrentPlayer = GetNextPlayer(DealingPlayer);

			IssueCommunalCards();
			std::cout << "1st, 2nd and 3rd Community Card are placed on board..." << std::endl;
			break;
		}
		case Phase::Turn:
		{
			std::cout << "Current Phase: Turn" << std::endl;

			CurrentPlayer = GetNextPlayer(DealingPlayer);

			IssueCommunalCards();
			std::cout << "4th Community Card is placed on board..." << std::endl;
			break;
		}
		case Phase::River:
		{
			std::cout << "Current Phase: River" << std::endl;

			CurrentPlayer = GetNextPlayer(DealingPlayer);

			IssueCommunalCards();
			std::cout << "5th Community Card are placed on board..." << std::endl;
			break;
		}
	}

	Print();

	std::cout << "========================================" << std::endl;
}

void Board::UpdatePhase()
{
	if (IsRoundEnded() || (IsPhaseEnded() && CurrentState == Phase::River))
	{
		EndRound();
		return;
	}
	else if (IsPhaseEnded() && CurrentState != Phase::River)
	{
		NextPhase();
	}
	
	CurrentPlayer->Update();

	if (CurrentPlayer->GetAction() != BettingAction::NONE)
	{
		switch(CurrentPlayer->GetAction())
		{
			case BettingAction::Fold:
			{
				std::cout << "Player " << CurrentPlayer->GetIndex() << " has folded." << std::endl;
				CurrentPlayer->SetIsFolded(true);
				break;
			}
			case BettingAction::Check:
			{
				std::cout << "Player " << CurrentPlayer->GetIndex() << " has checked." << std::endl;
				break;
			}
			case BettingAction::Call:
			{
				std::cout << "Player " << CurrentPlayer->GetIndex() << " has called to $" << RequiredAnte << std::endl;
				CurrentPlayer->SetAnte(RequiredAnte);
				UpdatePot();

				if (CurrentPlayer->GetStack() == 0) CurrentPlayer->SetIsParticipating(false);
				break;
			}
			case BettingAction::Raise:
			{
				unsigned int RaiseAmt = BigBlind;
				if (CurrentState == Phase::River || CurrentState == Phase::Turn) RaiseAmt *= 2;

				CurrentPlayer->SetAnte(CurrentPlayer->GetAnte() + RaiseAmt);
				RequiredAnte = CurrentPlayer->GetAnte();

				std::cout << "Player " << CurrentPlayer->GetIndex() << " has raised to $" << RequiredAnte << std::endl;
				if (CurrentPlayer->GetStack() == 0) CurrentPlayer->SetIsParticipating(false);
				break;
			}
			case BettingAction::Bet:
			{
				unsigned int BetAmt = BigBlind;
				if (CurrentState == Phase::River || CurrentState == Phase::Turn) BetAmt *= 2;

				CurrentPlayer->SetAnte(CurrentPlayer->GetAnte() + BetAmt);
				RequiredAnte = CurrentPlayer->GetAnte();

				std::cout << "Player " << CurrentPlayer->GetIndex() << " has bet for $" << RequiredAnte << std::endl;
				if (CurrentPlayer->GetStack() == 0) CurrentPlayer->SetIsParticipating(false);
				break;
			}
			default:
			{
				std::cout << "Current betting action is invalid." << std::endl;
				break;
			}
		}
	}

	CurrentPlayer = GetNextPlayer(CurrentPlayer);
}

void Board::NextPhase()
{
	CurrentState = static_cast<Phase>(static_cast<int>(CurrentState) + 1);
	StartPhase();
}

bool Board::IsPhaseEnded()
{
	for (unsigned int Index = 0; Index < Players.size(); Index++)
	{
		if (!Players[Index]->GetIsBroke() && !Players[Index]->GetIsFolded() && Players[Index]->GetStack() > 0 && Players[Index]->GetIsParticipating())
		{
			if (CurrentState == Phase::Preflop && Players[Index]->GetAnte() < RequiredAnte)
				return false;

			else if (CurrentState != Phase::Preflop && (Players[Index]->GetAction() == BettingAction::NONE || Players[Index]->GetAnte() < RequiredAnte))
				return false;
		}
	}

	return true;
}

bool Board::IsRoundEnded()
{
	unsigned int Count = 0;

	for (unsigned int Index = 0; Index < Players.size(); Index++)
		if (!Players[Index]->GetIsFolded() && !Players[Index]->GetIsBroke()) Count++;

	return Count == 1 ? true : false;
}

bool Board::IsGameEnded()
{
	unsigned int Count = 0;
	
	for (unsigned int Index = 0; Index < Players.size(); Index++)
		if (!Players[Index]->GetIsBroke()) Count++;
	
	return Count <= 1 ? true : false;
}

void Board::AddPlayer(Player* _AddingPlayer)
{
	Players.push_back(_AddingPlayer);
	_AddingPlayer->SetStack(EntryStack);
}

void Board::AddPlayer(Player* _AddingPlayer, unsigned int _EntryStack)
{
	Players.push_back(_AddingPlayer);
	_AddingPlayer->SetStack(_EntryStack);
}

void Board::RemovePlayer(Player* _RemovingPlayer)
{
	Players.erase(Players.begin() + (_RemovingPlayer->GetIndex() - 1));
}

Player* Board::GetPreviousPlayer(Player* _Reference)
{
	if (_Reference->GetIndex() == 0)
		return Players[Players.size() - 1];

	return Players[_Reference->GetIndex() - 1];
}

Player* Board::GetNextPlayer(Player* _Reference)
{
	if (_Reference->GetIndex() == Players.size() - 1)
		return Players[0];

	return Players[_Reference->GetIndex() + 1];
}

void Board::UpdatePot()
{
	EmptyPot();

	for (unsigned int Index = 0; Index < Players.size(); Index++)
		Pot += Players[Index]->GetPotContribution();
}

void Board::EmptyPot()
{
	Pot = 0;
}

void Board::SplitPot(std::vector<unsigned int> &_Pots, std::vector<std::vector<Player*>> &_ValidPlayersPerPot)
{
	std::vector<unsigned int> EntryValues;
	std::vector<Player*> BettingPlayers = GetNonBrokePlayers();

	//Sort BettingPlayers by their contributions in ascending order
	std::vector<Player*>::iterator NextPlayer;
	std::vector<Player*>::iterator UpperPlayer;

	for (auto PlayerItr = BettingPlayers.begin(); PlayerItr != BettingPlayers.end(); PlayerItr++)
	{
		NextPlayer = std::next(PlayerItr);
		
		UpperPlayer = PlayerItr;
		for (auto UpperItr = BettingPlayers.begin(); UpperItr != PlayerItr; UpperItr++)
		{
			if ((*UpperItr)->GetPotContribution() > (*PlayerItr)->GetPotContribution())
			{
				UpperPlayer = UpperItr;
				break;
			}
		}

		std::rotate(UpperPlayer, PlayerItr, NextPlayer);
	}

	//Loop through the BettingPlayers to determine the size of pots and it's valid players
	for (auto PlayerItr = BettingPlayers.begin(); PlayerItr != std::prev(BettingPlayers.end()); PlayerItr++)
	{
		if ((*PlayerItr)->GetIsBroke())
			continue;

		std::vector<Player*> ContributingPlayers;
		unsigned int EntryValue = (*PlayerItr)->GetPotContribution();
		EntryValues.push_back(EntryValue);

		for (unsigned int ContriIndex = 0; ContriIndex < BettingPlayers.size(); ContriIndex++)
		{
			if (BettingPlayers[ContriIndex]->GetIsFolded() && ((_Pots.size() == 0 && BettingPlayers[ContriIndex]->GetPotContribution() > EntryValue) || (_Pots.size() > 0 && (BettingPlayers[ContriIndex]->GetPotContribution() - _Pots[_Pots.size() - 1]) > EntryValue)))
				ContributingPlayers.push_back(BettingPlayers[ContriIndex]);

			else if (!BettingPlayers[ContriIndex]->GetIsFolded() && BettingPlayers[ContriIndex]->GetPotContribution() >= EntryValue)
				ContributingPlayers.push_back(BettingPlayers[ContriIndex]);
		}

		unsigned int PotSize = 0;
		for (unsigned int ContriIndex = 0; ContriIndex < ContributingPlayers.size(); ContriIndex++)
		{
			if (!BettingPlayers[ContriIndex]->GetIsFolded())
			{
				PotSize += _Pots.size() == 0 ? EntryValue : (EntryValue - EntryValues[_Pots.size() - 1]);
			}
			else
			{
				if (EntryValue > BettingPlayers[ContriIndex]->GetPotContribution())
					PotSize += BettingPlayers[ContriIndex]->GetPotContribution() - EntryValues[_Pots.size() - 1];
				else
					PotSize += EntryValue - EntryValues[_Pots.size() - 1];
			}
		}

		_Pots.push_back(PotSize);

		std::vector<Player*> ValidPlayersInPot;
		for (unsigned int ContriIndex = 0; ContriIndex < ContributingPlayers.size(); ContriIndex++)
		{
			if (ContributingPlayers[ContriIndex]->GetPotContribution() >= EntryValue)
				ValidPlayersInPot.push_back(ContributingPlayers[ContriIndex]);
		}

		//Re-arrange the players based on their Index
		for (auto PlayerItr = ValidPlayersInPot.begin(); PlayerItr != ValidPlayersInPot.end(); PlayerItr++)
		{
			NextPlayer = std::next(PlayerItr);

			UpperPlayer = PlayerItr;
			for (auto UpperItr = ValidPlayersInPot.begin(); UpperItr != PlayerItr; UpperItr++)
			{
				if ((*UpperItr)->GetIndex() > (*PlayerItr)->GetIndex())
				{
					UpperPlayer = UpperItr;
					break;
				}
			}

			std::rotate(UpperPlayer, PlayerItr, NextPlayer);
		}

		_ValidPlayersPerPot.push_back(ValidPlayersInPot);
	}

	//Check for leftover money in case of overbetting players
	int LeftOver = Pot;
	for (auto itr = _Pots.begin(); itr != _Pots.end(); itr++)
		LeftOver -= *itr;

	if (LeftOver > 0)
	{
		_Pots.push_back(LeftOver);

		std::vector<Player*> OverBetters;
		OverBetters.push_back(BettingPlayers[BettingPlayers.size() - 1]);

		_ValidPlayersPerPot.push_back(OverBetters);
	}
}

void Board::DealCardsToPlayers()
{
	PlayingDeck->Refill();
	PlayingDeck->Shuffle();

	for (unsigned int Index = 0; Index < Players.size(); Index++)
	{
		Players[Index]->EmptyHand();
		Players[Index]->SetHand(PlayingDeck->Draw(), PlayingDeck->Draw());
	}
}

void Board::IssueCommunalCards()
{
	switch(CurrentState)
	{
		case Phase::Flop:
		{
			for (unsigned int Index = 0; Index < 3; Index++)
				CommunalCards[Index] = PlayingDeck->Draw();
			break;
		}
		case Phase::Turn:
		{
			CommunalCards[3] = PlayingDeck->Draw();
			break;
		}
		case Phase::River:
		{
			CommunalCards[4] = PlayingDeck->Draw();
			break;
		}
		default:
		{
			std::cout << "Communal Cards cannot be issued in this BoardState." << std::endl;
			break;
		}
	}
}

void Board::ClearCommunalCards()
{
	for (unsigned int Index = 0; Index < 5; Index++)
		CommunalCards[Index] = nullptr;
}

std::vector<Player*> Board::GetNonBrokePlayers()
{
	std::vector<Player*> NonBrokePlayers;

	for (unsigned int Index = 0; Index < Players.size(); Index++)
		if (!Players[Index]->GetIsBroke()) NonBrokePlayers.push_back(Players[Index]);

	return NonBrokePlayers;
}

std::vector<Player*> Board::GetBettingPlayers()
{
	std::vector<Player*> BettingPlayers;

	for (unsigned int Index = 0; Index < Players.size(); Index++)
		if (!Players[Index]->GetIsBroke() && !Players[Index]->GetIsFolded()) BettingPlayers.push_back(Players[Index]);

	return BettingPlayers;
}

std::vector<Player*> Board::DetermineWinningPlayers(std::vector<Player*> _Participants)
{
	std::vector<Player*> BettingPlayers = _Participants;

	if (BettingPlayers.size() == 1)
		return BettingPlayers;

	std::vector<std::array<Card*,5>> BettingHands;

	for (unsigned int Index = 0; Index < BettingPlayers.size(); Index++)
	{
		std::array<Card*, 5> PlayerBest = Evaluator->GetBestCommunalHand(BettingPlayers[Index]->GetHand(), CommunalCards);
		BettingHands.push_back(PlayerBest);
		
		std::cout << "Player " << Index << "'s best hand: " << Evaluator->GetStr(PlayerBest) << " (" << Evaluator->GetTypeStr(PlayerBest) << ")" << " => " << Evaluator->DetermineValue_5Cards(PlayerBest) << std::endl;
	}

	std::array<Card*,5> BestHand = BettingHands[0];

	std::vector<Player*> WinningPlayers;
	WinningPlayers.push_back(BettingPlayers[0]);

	for (unsigned int Index = 1; Index < BettingHands.size(); Index++)
	{
		//std::cout << "Player " << BettingPlayers[Index]->GetIndex() << ": " << Evaluator->GetStr(BettingHands[Index]) << " (" << Evaluator->GetTypeStr(BettingHands[Index]) << ")  /  Best Hand: " << Evaluator->GetStr(BestHand) << " (" << Evaluator->GetTypeStr(BestHand) << ")" << std::endl;

		switch (Evaluator->IsBetter5Cards(BettingHands[Index],BestHand))
		{
			case ComparisonResult::Win:
			{
				BestHand = BettingHands[Index];

				WinningPlayers.clear();
				WinningPlayers.push_back(BettingPlayers[Index]);	
				break;
			}
			case ComparisonResult::Draw:
			{
				WinningPlayers.push_back(BettingPlayers[Index]);
				break;
			}
			default:
				break;
		}
	}

	return WinningPlayers;
}

void Board::AwardPlayer(Player* _Player, unsigned int _Amt)
{
	_Player->SetStack(_Player->GetStack() + _Amt);
}

void Board::Print()
{
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	
	std::cout << "Community Cards: ";
	for(unsigned int Index = 0; Index < 5; Index++)
	{
		if (CommunalCards[Index] != nullptr)
			std::cout << " " << CommunalCards[Index]->GetInfo();	
		else
			break;
	}

	std::cout << std::endl  << std::endl;

	std::cout << "Pot: $" << Pot << std::endl << std::endl;

	for (unsigned int Index = 0; Index < Players.size(); Index++)
	{
		if (Players[Index]->GetIsBroke())
			std::cout << "Player " << Index << " ($" << Players[Index]->GetStack() << "): " << ": BROKE" << std::endl;
		else if (Players[Index]->GetIsFolded())
			std::cout << "Player " << Index << " ($" << Players[Index]->GetStack() << "): " << ": FOLD" << std::endl;
		else if (!Players[Index]->GetIsFolded() && !Players[Index]->GetIsParticipating())
			std::cout << "Player " << Index << " ($" << Players[Index]->GetStack() << "): " << ": ALL-IN" << "   /   $" << Players[Index]->GetAnte() << std::endl;
		else
			std::cout << "Player " << Index << " ($" << Players[Index]->GetStack() << "): " << Players[Index]->GetHand()[0]->GetInfo() << "," << Players[Index]->GetHand()[1]->GetInfo() << "  /  $" << Players[Index]->GetAnte() << std::endl;
	}

	std::cout << "--------------------------------------------------------------------------------" << std::endl;
}

