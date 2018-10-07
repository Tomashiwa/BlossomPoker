#include "Board.h"
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "HandEvaluator.h"

#include <algorithm>
#include <iterator>

Board::Board(std::shared_ptr<HandEvaluator> _Evaluator, unsigned int _BigBlind, bool _PrintProcess)
	: SmallBlind(_BigBlind/2), BigBlind(_BigBlind), RequiredAnte(BigBlind), EntryStack(100 * _BigBlind), Pot(0), Round(1),
	  Evaluator(_Evaluator), 
	  PrintProcess(_PrintProcess)
{
	PlayingDeck = std::make_unique<Deck>();
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
	//printf("read %zu bytes\n", bytesread * sizeof(*HR));
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
	StartRound();
}

void Board::Update()
{
	if (IsActive)
		UpdateRound();
}

void Board::End()
{
	for (auto const& CurrentPlayer : Players)
	{
		if (!CurrentPlayer->GetIsBroke())
			std::cout << "Winner: P." << CurrentPlayer->GetIndex() << " ($" << CurrentPlayer->GetStack() << ") \n";
	}
}

void Board::Reset(bool _IsHard)
{
	IsActive = true;
	CurrentState = Phase::Preflop;

	for (auto& Player : Players)
		Player->Reset();

	RemoveAllPlayers();
	DealingPlayer.reset();
	BigBlindPlayer.reset();
	SmallBlindPlayer.reset();
	CurrentPlayer.reset();

	PlayingDeck->Refill();
	PlayingDeck->Shuffle();

	for (auto& Card : CommunalCards)
		Card.reset();

	Pot = 0;
	RequiredAnte = 0;

	if (_IsHard)
	{
		Records.clear();
		Round = 0;
	}
}

void Board::SoftReset()
{
	IsActive = true;
	CurrentState = Phase::Preflop;

}

void Board::ResetRecords()
{
	for (auto Itr = Records.begin(); Itr != Records.end(); ++Itr)
		Itr->second = 0;
}

void Board::ResetStacks()
{
	for (auto const& Player : Players)
	{
		Player->SetStack(EntryStack);
		Player->SetIsBroke(false);
	}

	Pot = 0;
	RequiredAnte = 0;
}

void Board::StartRound()
{
	CurrentState = Phase::Preflop;
	Round += 1;

	for (auto const& Player : Players)
	{
		if (!Player->GetIsBroke())
			Player->SetIsParticipating(true);

		Player->EmptyPotContributon();
	}

	DealingPlayer = !DealingPlayer ? Players[0] : GetNextPlayer(DealingPlayer);
	SmallBlindPlayer = GetNextPlayer(DealingPlayer);
	BigBlindPlayer = GetNextPlayer(SmallBlindPlayer);

	if(PrintProcess)
		std::cout << "Round " << Round << ": \n";

	StartPhase();
}

void Board::UpdateRound()
{
	UpdatePhase();
}

void Board::EndRound()
{
	std::vector<unsigned int> Pots;
	std::vector<std::vector<std::shared_ptr<Player>>> ValidPlayersPerPot;

	SplitPot(Pots, ValidPlayersPerPot);

	std::map<std::shared_ptr<Player>, unsigned int> PlayersWinning;
	for (auto& Player : Players)
		PlayersWinning.insert(std::make_pair(Player, 0));

	std::vector<std::shared_ptr<Player>> PotWinners;

	for (unsigned int PotIndex = 0; PotIndex < Pots.size(); PotIndex++)
	{
		//std::cout << "Pot: $" << Pots[PotIndex] << " / Participants: ";
		//for (int Index = 0; Index < ValidPlayersPerPot[PotIndex].size(); Index++)
		//	std::cout << "P." << ValidPlayersPerPot[PotIndex][Index]->GetIndex() << " ($" << ValidPlayersPerPot[PotIndex][Index]->GetPotContribution() << ")  ";
		//std::cout << std::endl;
		
		if (ValidPlayersPerPot[PotIndex].size() == 1)
		{
			AwardPlayer(ValidPlayersPerPot[PotIndex][0], Pots[PotIndex]);
			
			if(PrintProcess)
				std::cout << "P." << ValidPlayersPerPot[PotIndex][0]->GetIndex() << " win $" << Pots[PotIndex] << " from Pot " << PotIndex + 1 << " (Stack: $" << ValidPlayersPerPot[PotIndex][0]->GetStack() << ") \n";

			PlayersWinning[ValidPlayersPerPot[PotIndex][0]] += Pots[PotIndex];

			continue;
		}

		DetermineWinningPlayers(ValidPlayersPerPot[PotIndex], PotWinners);

		if (PotWinners.size() == 1)
		{
			AwardPlayer(PotWinners[0], Pots[PotIndex]);

			if(PrintProcess)
				std::cout << "P." << PotWinners[0]->GetIndex() << " win $" << Pots[PotIndex] << " from Pot " << PotIndex + 1 << " (Stack: $" << PotWinners[0]->GetStack() << ") \n";
		
			PlayersWinning[PotWinners[0]] += Pots[PotIndex];
		}

		else if (PotWinners.size() > 1)
		{
			unsigned int Portion = Pots[PotIndex] / PotWinners.size();

			for (unsigned int WinnerIndex = 0; WinnerIndex < PotWinners.size(); WinnerIndex++)
			{
				AwardPlayer(PotWinners[WinnerIndex], Portion);
				
				if(PrintProcess)
					std::cout << "P." << PotWinners[WinnerIndex]->GetIndex() << " win $" << Portion << " from Pot " << PotIndex + 1 << " (Stack: $" << PotWinners[WinnerIndex]->GetStack() << ") \n";
			
				PlayersWinning[PotWinners[WinnerIndex]] += Portion;
			}
		}

		PotWinners.clear();
	}

	auto Winner = PlayersWinning.begin();
	for (auto Itr = PlayersWinning.begin(); Itr != PlayersWinning.end(); ++Itr)
	{
		if (Itr->second > Winner->second)
			Winner = Itr;
	}
	Records[Winner->first] += 1;

	ClearCommunalCards();
	EmptyPot();

	for (auto const& Player : Players)
	{
		Player->EmptyHand();
		Player->SetAnte(0);
		Player->SetIsFolded(false);

		if (Player->GetStack() <= 0)
		{
			Player->SetIsBroke(true);

			if (PrintProcess)
				std::cout << "P." << Player->GetIndex() << " is broke... \n";
		}
	}

	if (IsGameEnded())
	{
		IsActive = false;
		//std::cout << "Game has ended" << std::endl;
		return;
	}

	if (PrintProcess)
	{
		std::cout << "Result: / ";
		for (unsigned int Index = 0; Index < Players.size(); Index++)
			std::cout << "P." << Players[Index]->GetIndex() << ": " << Players[Index]->GetStack() << " ";

		std::cout << "/ \n\n";
	}

	StartRound();
}

void Board::StartPhase()
{
	for (auto const& Player : Players)
	{
		Player->SetAction(BettingAction::NONE);
		Player->SetAnte(0);
	}

	RequiredAnte = 0;

	switch (CurrentState)
	{
		case Phase::Preflop:
		{
			if(PrintProcess)
				std::cout << "\nPhase: Pre-flop\n";
			
			SmallBlindPlayer->SetAnte(SmallBlind);
			BigBlindPlayer->SetAnte(BigBlind);
			CurrentPlayer = GetNextPlayer(BigBlindPlayer);

			RequiredAnte = BigBlind;
			UpdatePot();

			DealCardsToPlayers();
			break;
		}
		case Phase::Flop:
		{
			if (PrintProcess)
				std::cout << "\nPhase: Flop\n";

			CurrentPlayer = GetNextPlayer(DealingPlayer);
			IssueCommunalCards();
			break;
		}
		case Phase::Turn:
		{
			if (PrintProcess)
				std::cout << "\nPhase: Turn\n";

			CurrentPlayer = GetNextPlayer(DealingPlayer);
			IssueCommunalCards();
			break;
		}
		case Phase::River:
		{
			if (PrintProcess)
				std::cout << "\nPhase: River\n";

			CurrentPlayer = GetNextPlayer(DealingPlayer);
			IssueCommunalCards();
			break;
		}
	}

	if (PrintProcess)
	{
		for (auto const& Player : Players)
			std::cout << "P." << Player->GetIndex() << ": " << Evaluator->GetStr(Player->GetHand()) << "  ";
		std::cout << "\n";
		std::cout << "Board: " << Evaluator->GetStr(CommunalCards) << "\n";	
	}
}

void Board::UpdatePhase()
{
	//std::cout << "enter update phase\n";
	//std::cout << "testing for round/phase end...\n";
	if (IsRoundEnded() || (IsPhaseEnded() && CurrentState == Phase::River))
	{
		//std::cout << "ending round\n";
		//std::cout << "Round has ended... (Phase: " << GetStateStr() << " / Required A: " << GetRequiredAnte() << ") = Ended at " << GetStateStr() << "\n";
		EndRound();

		return;
	}
	else if (IsPhaseEnded() && CurrentState != Phase::River)
	{
		NextPhase();
		//std::cout << "Moving to next phase...(" << GetStateStr() << ")\n";
		
	}
	else if (CurrentPlayer->GetIsBroke() || CurrentPlayer->GetIsFolded() || CurrentPlayer->GetIsParticipating() == false)
	{
		//std::cout << "Getting next player...\n";
		CurrentPlayer = GetNextPlayer(CurrentPlayer);
		return;
	}
	
	//std::cout << "update pot\n";
	UpdatePot();
	//std::cout << "update player\n";
	CurrentPlayer->Update();

	//std::cout << "exectute player action\n";
	switch(CurrentPlayer->GetAction())
	{
		case BettingAction::Fold:
		{
			if (PrintProcess) 
				std::cout << "P." << CurrentPlayer->GetIndex() << " folded. \n";
			
			CurrentPlayer->SetIsFolded(true);
			break;
		}
		case BettingAction::Check:
		{
			if (PrintProcess)
				std::cout << "P." << CurrentPlayer->GetIndex() << " checked. \n";
			break;
		}
		case BettingAction::Call:
		{
			if (PrintProcess)
				std::cout << "P." << CurrentPlayer->GetIndex() << " called to $" << RequiredAnte << " (Pot: " << Pot << ") \n";
			
			CurrentPlayer->SetAnte(RequiredAnte);
			UpdatePot();

			if (CurrentPlayer->GetStack() <= 0)
			{
				CurrentPlayer->SetIsParticipating(false);
				//std::cout << "P." << CurrentPlayer->GetIndex() << " is no longer participating...\n";
			}
			
			break;
		}
		case BettingAction::Raise:
		{
			unsigned int RaiseAmt = BigBlind;
			if (CurrentState == Phase::River || CurrentState == Phase::Turn) RaiseAmt *= 2;

			RaiseAmt += RequiredAnte - CurrentPlayer->GetAnte();

			CurrentPlayer->SetAnte(CurrentPlayer->GetAnte() + RaiseAmt);
			RequiredAnte = CurrentPlayer->GetAnte();

			if (PrintProcess)
				std::cout << "P." << CurrentPlayer->GetIndex() << " raised to $" << RequiredAnte << " (Pot: " << Pot << ") \n";
			
			if (CurrentPlayer->GetStack() <= 0)
			{
				CurrentPlayer->SetIsParticipating(false);
				//std::cout << "P." << CurrentPlayer->GetIndex() << " is no longer participating...\n";
			}
			break;
		}
		case BettingAction::Bet:
		{
			unsigned int BetAmt = BigBlind;
			if (CurrentState == Phase::River || CurrentState == Phase::Turn) BetAmt *= 2;

			CurrentPlayer->SetAnte(CurrentPlayer->GetAnte() + BetAmt);
			RequiredAnte = CurrentPlayer->GetAnte();

			if (PrintProcess)
				std::cout << "P." << CurrentPlayer->GetIndex() << " bet $" << RequiredAnte << " (Pot: " << Pot << ") \n";
			
			if (CurrentPlayer->GetStack() <= 0)
			{
				CurrentPlayer->SetIsParticipating(false);
				//std::cout << "P." << CurrentPlayer->GetIndex() << " is no longer participating...\n";
			}
			break;
		}
		default:
		{
			std::cout << "Invalid BettingAction given. \n";
			break;
		}

	}

	//std::cout << "moving to next player\n";
	CurrentPlayer = GetNextPlayer(CurrentPlayer);
}

void Board::NextPhase()
{
	CurrentState = static_cast<Phase>(static_cast<int>(CurrentState) + 1);
	StartPhase();
}

bool Board::IsPhaseEnded()
{
	for (auto const& Player: Players)
	{
		if (!Player->GetIsBroke() && !Player->GetIsFolded() && Player->GetIsParticipating())
		{
			if (CurrentState == Phase::Preflop && Player->GetAnte() < RequiredAnte)
				return false;

			else if (CurrentState != Phase::Preflop && (Player->GetAction() == BettingAction::NONE || Player->GetAnte() < RequiredAnte))
				return false;
		}
	}

	return true;
}

bool Board::IsRoundEnded()
{
	unsigned int RemainingPlayers = 0;

	for (auto const& Player : Players)
		if (!Player->GetIsFolded() && !Player->GetIsBroke()) RemainingPlayers++;

	return RemainingPlayers <= 1 ? true : false;
}

bool Board::IsGameEnded()
{
	unsigned int Count = 0;
	
	for (unsigned int Index = 0; Index < Players.size(); Index++)
		if (!Players[Index]->GetIsBroke()) Count++;
	
	return Count <= 1 ? true : false;
}

void Board::AddPlayer(std::shared_ptr<Player> _AddingPlayer)
{
	Players.push_back(_AddingPlayer);
	Records.insert(std::make_pair(_AddingPlayer, 0));

	_AddingPlayer->SetStack(EntryStack);
}

void Board::AddPlayer(std::shared_ptr<Player> _AddingPlayer, unsigned int _EntryStack)
{
	Players.push_back(_AddingPlayer);
	Records.insert(std::make_pair(_AddingPlayer, 0));

	_AddingPlayer->SetStack(_EntryStack);
}

void Board::RemovePlayer(std::shared_ptr<Player>& _RemovingPlayer)
{
	Players.erase(Players.begin() + (_RemovingPlayer->GetIndex() - 1));
	_RemovingPlayer.reset();
}

void Board::RemoveAllPlayers()
{
	Players.clear();
}

std::shared_ptr<Player> Board::GetPreviousPlayer(const std::shared_ptr<Player>& _Reference)
{
	for (unsigned int Index = 0; Index < Players.size(); Index++)
	{
		if (Players[Index] == _Reference)
			return Index <= 0 ? Players[Players.size() - 1] : Players[Index - 1];
	}

	return nullptr;

	/*for (auto Itr = Players.begin(); Itr != Players.end(); ++Itr)
	{
		if ((*Itr)->GetIndex() == _Reference->GetIndex())
			return Itr == Players.begin() ? *(--Players.end()) : *(--Itr);
	}

	return nullptr;*/
}

std::shared_ptr<Player> Board::GetNextPlayer(const std::shared_ptr<Player>& _Reference)
{
	for (unsigned int Index = 0; Index < Players.size(); Index++)
	{
		if (Players[Index] == _Reference)
			return Index >= (Players.size() - 1) ? Players[0] : Players[Index + 1];
	}

	return nullptr;

	/*for (auto Itr = Players.begin(); Itr != Players.end(); ++Itr)
	{
		if ((*Itr)->GetIndex() == _Reference->GetIndex())
			return Itr == (--Players.end()) ? *Players.begin() : *(++Itr);
	}

	return nullptr;*/
}

void Board::UpdatePot()
{
	EmptyPot();

	for (auto const& Player : Players)
		Pot += Player->GetPotContribution();
}

void Board::EmptyPot()
{
	Pot = 0;
}

void Board::SplitPot(std::vector<unsigned int> &_Pots, std::vector<std::vector<std::shared_ptr<Player>>> &_ValidPlayersPerPot)
{
	std::vector<unsigned int> EntryValues;
	std::vector<std::shared_ptr<Player>> ParticipatingPlayers;
	GetParticipatingPlayers(ParticipatingPlayers);
	
	//Sort ParticipatingPlayers by their contributions in ascending order
	std::vector<std::shared_ptr<Player>>::iterator NextPlayer;
	std::vector<std::shared_ptr<Player>>::iterator UpperPlayer;

	for (auto PlayerItr = ParticipatingPlayers.begin(); PlayerItr != ParticipatingPlayers.end(); PlayerItr++)
	{
		NextPlayer = std::next(PlayerItr);
		
		UpperPlayer = PlayerItr;
		for (auto UpperItr = ParticipatingPlayers.begin(); UpperItr != PlayerItr; UpperItr++)
		{
			if ((*UpperItr)->GetPotContribution() > (*PlayerItr)->GetPotContribution())
			{
				UpperPlayer = UpperItr;
				break;
			}
		}

		std::rotate(UpperPlayer, PlayerItr, NextPlayer);
	}

	std::vector<std::shared_ptr<Player>> ContributingPlayers;
	std::vector<std::shared_ptr<Player>> ValidPlayersInPot;
	unsigned int EntryValue, PotSize;

	//Loop through the ParticipatingPlayers to determine the size of pots and it's valid players
	for (auto const& Participant : ParticipatingPlayers)//auto PlayerItr = ParticipatingPlayers.begin(); PlayerItr != std::prev(ParticipatingPlayers.end()); PlayerItr++)
	{
		if (Participant->GetIsBroke())//(*PlayerItr)->GetIsBroke())
			continue;

		EntryValue = Participant->GetPotContribution();
		EntryValues.push_back(EntryValue);

		for (auto const& PotentialContributor : ParticipatingPlayers) //unsigned int ContriIndex = 0; ContriIndex < ParticipatingPlayers.size(); ContriIndex++)
		{
			if (PotentialContributor->GetIsFolded() && ((_Pots.size() == 0 && PotentialContributor->GetPotContribution() > EntryValue) || (_Pots.size() > 0 && (PotentialContributor->GetPotContribution() - _Pots[_Pots.size() - 1]) > EntryValue)))
				ContributingPlayers.push_back(PotentialContributor);

			else if (!PotentialContributor->GetIsFolded() && PotentialContributor->GetPotContribution() >= EntryValue)
				ContributingPlayers.push_back(PotentialContributor);
		}

		PotSize = 0;
		for (auto const& Contributor : ContributingPlayers)//unsigned int ContriIndex = 0; ContriIndex < ContributingPlayers.size(); ContriIndex++)
		{
			if (!Contributor->GetIsFolded())
			{
				PotSize += _Pots.size() == 0 ? EntryValue : (EntryValue - EntryValues[_Pots.size() - 1]);
			}
			else
			{
				if (EntryValue > Contributor->GetPotContribution())
					PotSize += Contributor->GetPotContribution() - EntryValues[_Pots.size() - 1];
				else
					PotSize += _Pots.size() == 0 ? EntryValue : (EntryValue - EntryValues[_Pots.size() - 1]);
			}
		}

		_Pots.push_back(PotSize);
		for (auto const& Contributor : ContributingPlayers)//unsigned int ContriIndex = 0; ContriIndex < ContributingPlayers.size(); ContriIndex++)
		{
			if (Contributor->GetPotContribution() >= EntryValue)
				ValidPlayersInPot.push_back(Contributor);
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

		ContributingPlayers.clear();
		ValidPlayersInPot.clear();
	}

	//Check for leftover money in case of overbetting players
	int LeftOver = Pot;
	for (auto itr = _Pots.begin(); itr != _Pots.end(); itr++)
		LeftOver -= *itr;

	if (LeftOver > 0)
	{
		_Pots.push_back(LeftOver);

		std::vector<std::shared_ptr<Player>> OverBetters;
		OverBetters.push_back(ParticipatingPlayers[ParticipatingPlayers.size() - 1]);

		_ValidPlayersPerPot.push_back(OverBetters);
	}
}

void Board::DealCardsToPlayers()
{
	PlayingDeck->Refill();
	PlayingDeck->Shuffle();

	for (auto const& Player : Players)
	{
		Player->EmptyHand();
		Player->SetHand(PlayingDeck->Draw(), PlayingDeck->Draw());
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
			std::cout << "Communal Cards cannot be issued in this state." << std::endl;
			break;
		}
	}
}

void Board::ClearCommunalCards()
{
	for (auto& Card : CommunalCards)
		Card = nullptr;
}

void Board::GetParticipatingPlayers(std::vector<std::shared_ptr<Player>>& _Participants)
{
	for (auto& Player : Players)
	{
		if (!Player->GetIsBroke() && !Player->GetIsFolded()) 
			_Participants.push_back(Player);
	}
}

void Board::GetBettingPlayers(std::vector<std::shared_ptr<Player>>& _BettingPlayers)
{
	for (auto& Player : Players)
		if (!Player->GetIsBroke() && !Player->GetIsFolded()) _BettingPlayers.push_back(Player);
}

void Board::DetermineWinningPlayers(const std::vector<std::shared_ptr<Player>>& _Participants, std::vector<std::shared_ptr<Player>>& _Winners)
{
	if (_Participants.size() == 1)
	{
		_Winners = _Participants;
		return;
	}

	std::vector<std::array<std::shared_ptr<Card>,5>> BettingHands;

	for (unsigned int Index = 0; Index < _Participants.size(); Index++)
	{
		BettingHands.push_back(Evaluator->GetBestCommunalHand(_Participants[Index]->GetHand(), CommunalCards));
	}

	std::array<std::shared_ptr<Card>,5> BestHand = BettingHands[0];

	_Winners.push_back(_Participants[0]);

	for (unsigned int Index = 1; Index < BettingHands.size(); Index++)
	{
		switch (Evaluator->IsBetter5Cards(BettingHands[Index],BestHand))
		{
			case ComparisonResult::Win:
			{
				BestHand = BettingHands[Index];

				_Winners.clear();
				_Winners.push_back(_Participants[Index]);
				break;
			}
			case ComparisonResult::Draw:
			{
				_Winners.push_back(_Participants[Index]);
				break;
			}
			default:
				break;
		}
	}
}

void Board::AwardPlayer(const std::shared_ptr<Player>& _Player, unsigned int _Amt)
{
	_Player->SetStack(_Player->GetStack() + _Amt);
}

void Board::Print()
{
	std::cout << "\n--------------------------------------------------\n";
	
	std::cout << "Board: ";
	for(unsigned int Index = 0; Index < 5; Index++)
	{
		if (CommunalCards[Index] != nullptr)
			std::cout << " " << CommunalCards[Index]->GetInfo();	
		else
			break;
	}

	std::cout << "\nPot: $" << Pot << "\n \n";

	for (unsigned int Index = 0; Index < Players.size(); Index++)
	{
		if (Players[Index]->GetIsBroke())
			std::cout << "P." << Index << " ($" << Players[Index]->GetStack() << "): " << ": Broke \n";
		else if (Players[Index]->GetIsFolded())
			std::cout << "P." << Index << " ($" << Players[Index]->GetStack() << "): " << ": Fold \n";
		else if (!Players[Index]->GetIsFolded() && !Players[Index]->GetIsParticipating())
			std::cout << "P." << Index << " ($" << Players[Index]->GetStack() << "): " << ": All-in \n" << "   |   $" << Players[Index]->GetAnte() << "\n";
		else
			std::cout << "P." << Index << " ($" << Players[Index]->GetStack() << "): " << Players[Index]->GetHandCardByIndex(0)->GetInfo() << "," << Players[Index]->GetHand()[1]->GetInfo() << "  |  $" << Players[Index]->GetAnte() << "\n";
	}

	std::cout << "--------------------------------------------------\n\n";
}

std::string Board::GetStateStr()
{
	switch (CurrentState)
	{
		case Phase::Preflop:
			return "Preflop";
		case Phase::Flop:
			return "Flop";
		case Phase::Turn:
			return "Turn";
		case Phase::River:
			return "River";
		default:
			break;
	}

	return "";
}
