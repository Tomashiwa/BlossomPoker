#include "Table.h"
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "HandEvaluator.h"

#include <algorithm>
#include <iterator>

Table::Table(std::shared_ptr<HandEvaluator> _Evaluator, unsigned int _BigBlind, bool _PrintProcess)
	: SmallBlind(_BigBlind/2), BigBlind(_BigBlind), RequiredAnte(BigBlind), EntryStack(100 * _BigBlind), Pot(0), Round(1),
	  Evaluator(_Evaluator), 
	  PrintProcess(_PrintProcess)
{
	ActiveDeck = std::make_unique<Deck>();
	ArrangedDeck = std::make_unique<Deck>();
}

Table::~Table()
{
}

void Table::InitEvaluator()
{
	memset(HR, 0, sizeof(HR));
	FILE * fin = fopen("HANDRANKS.DAT", "rb");

	// Load the HANDRANKS.DAT file data into the HR array
	size_t bytesread = fread(HR, sizeof(HR), 1, fin);
	fclose(fin);
}

int Table::GetHandValue(int* _Cards)
{
	int *p1 = HR + HR[53 + _Cards[0]];
	int *p2 = HR + p1[_Cards[1]];
	int *p3 = HR + p2[_Cards[2]];
	int *p4 = HR + p3[_Cards[3]];
	int *p5 = HR + p4[_Cards[4]];

	return p5[0];
}

void Table::Run()
{
	Start();

	FirstPlayer = DealingPlayer;
	
	while (!IsGameEnded())
		Update();

	End();
}

void Table::Start()
{
	StartRound();
}

void Table::Update()
{
	if (IsActive)
		UpdateRound();
}

void Table::End()
{
	for (auto const& CurrentPlayer : Players)
	{
		if (!CurrentPlayer->GetIsBroke())
			std::cout << "Winner: P." << CurrentPlayer->GetIndex() << " ($" << CurrentPlayer->GetStack() << ") \n";
	}
}

void Table::Reset(bool _IsHard)
{
	IsActive = true;
	
	Round = 0;
	CurrentState = Phase::Preflop;

	Pot = 0;
	RequiredAnte = 0;

	for (auto& Card : CommunalCards)
		Card.reset();

	ActiveDeck->Refill();
	ActiveDeck->Shuffle();

	if (_IsHard)
	{
		for (auto const& Player : Players)
			Player->Reset();

		RemoveAllPlayers();
		DealingPlayer.reset();
		BigBlindPlayer.reset();
		SmallBlindPlayer.reset();
		CurrentPlayer.reset();

		FirstPlayer.reset();
	}
	else
	{
		for (auto const& Player : Players)
		{
			Player->SetStack(EntryStack);
			Player->SetAnte(0);
			Player->EmptyPotContributon();
			Player->SetIsBroke(false);
			Player->SetIsFolded(false);
			Player->SetIsContributing(true);
		}
	}
}

void Table::StartRound()
{
	CurrentState = Phase::Preflop;
	Round += 1;

	for (auto const& Player : Players)
	{
		if (!Player->GetIsBroke())
			Player->SetIsContributing(true);

		Player->EmptyPotContributon();
	}

	DealingPlayer = !DealingPlayer ? Players[0] : GetNextPlayer(DealingPlayer);
	SmallBlindPlayer = GetNextPlayer(DealingPlayer);
	BigBlindPlayer = GetNextPlayer(SmallBlindPlayer);

	if (PrintProcess)
	{
		std::cout << "------------------------------------------------------------------------------------\n";
		std::cout << "Round " << Round << ": \n";
	}

	StartPhase();
}

void Table::UpdateRound()
{
	UpdatePhase();
}

void Table::EndRound()
{
	std::cout << "Round " << Round << ":\n";
	DistributeWinnings();

	int HighestProfit = CurrentMatch->RankingBoard[0]->Profits;
	for (auto const& Participant : CurrentMatch->RankingBoard) 
	{
		if (!Participant->Owner->GetIsBroke() && !Participant->Owner->GetIsFolded() && Participant->Profits > HighestProfit)
			HighestProfit = Participant->Profits;
	}

	for (auto const& Participant : CurrentMatch->RankingBoard)
	{
		if(!Participant->Owner->GetIsBroke())
			std::cout << "P." << Participant->Owner->GetIndex() << ": " << Participant->Profits << " (IsFolded: " << Participant->Owner->GetIsFolded() << ") \n";
	} 

	std::cout << "Highest profit: " << HighestProfit << "\n";
	if (HighestProfit == 0)
	{
		std::cout << "HighestProfit was found to be 0, Printing out all Player's profit and pot contribution...\n";
		std::cout << "Pot: " << Pot << "\n";

		for (auto const& Participant : CurrentMatch->RankingBoard)
			std::cout << "P." << Participant->Owner->GetIndex() << ": " << Participant->Profits << "(Profits), " << Participant->Owner->GetPotContribution() << " (Pot Contri)\n";
		std::cout << "\n";
	}

	int Count = 0;
	for (auto const& Participant : CurrentMatch->RankingBoard)
	{
		if (Participant->Profits == HighestProfit)
		{
			Participant->MoneyWon += Participant->Profits;
			Participant->HandsWon += 1;
			std::cout << "P." << Participant->Owner->GetIndex() << " won this hand...\n";
			
			Count++;
		}
	}

	if (Count >= 2)
		std::cout << "break\n";

	for (auto const& Participant : CurrentMatch->RankingBoard)
	{
		if (!Participant->Owner->GetIsBroke() && Participant->Profits < HighestProfit)
		{
			Participant->HandsLost += 1;
			if (Participant->Profits > 0)
				Participant->MoneyWon += Participant->Profits;
			else
				Participant->MoneyLost += -(Participant->Profits);

			std::cout << "Player " << Participant->Owner->GetIndex() << " lost this hand...\n";
		}
	}

	for (auto const& Participant : CurrentMatch->RankingBoard)
		Participant->Profits = 0;

	/*std::vector<unsigned int> Pots;
	std::vector<std::vector<std::shared_ptr<Player>>> ValidPlayersPerPot;
	std::vector<std::shared_ptr<Player>> PotWinners;

	std::shared_ptr<Participant> TargetParticipant;
	std::shared_ptr<Participant> LosingParticipant;

	unsigned int ContriTillNow = 0;
	unsigned int ContriNeeded = 0;
	
	SplitPot(Pots, ValidPlayersPerPot);

	//Folded Players
	for (auto const& Participant : CurrentMatch->RankingBoard)
	{
		if (!Participant->Owner->GetIsBroke() && Participant->Owner->GetIsFolded())
		{
			Participant->Profits -= Participant->Owner->GetPotContribution();
			Participant->MoneyLost += Participant->Owner->GetPotContribution();
		
			Participant->HandsLost += 1;
		}
	}

	//Go through each pot
	for (unsigned int PotIndex = 0; PotIndex < Pots.size(); PotIndex++)
	{
		ContriNeeded = Pots[PotIndex] / ValidPlayersPerPot[PotIndex].size();

		//Pot w/ only 1 valid player
		if (ValidPlayersPerPot[PotIndex].size() == 1)
		{
			AwardPlayer(ValidPlayersPerPot[PotIndex][0], Pots[PotIndex]);

			CurrentMatch->GetParticipant(ValidPlayersPerPot[PotIndex][0], TargetParticipant);
			TargetParticipant->Profits += Pots[PotIndex];
			TargetParticipant->MoneyWon += Pots[PotIndex];

			continue;
		}

		DetermineWinningPlayers(ValidPlayersPerPot[PotIndex], PotWinners);

		//Pot w/ multiple valid players
		//1 winner
		if (PotWinners.size() == 1)
		{
			AwardPlayer(PotWinners[0], Pots[PotIndex]);

			CurrentMatch->GetParticipant(ValidPlayersPerPot[PotIndex][0], TargetParticipant);
			TargetParticipant->Profits += Pots[PotIndex];
			TargetParticipant->MoneyWon += Pots[PotIndex];

			//De-reward all other players that stayed to the last phase
			for (auto const Player : ValidPlayersPerPot[PotIndex])
			{
				if (Player->GetIndex() != TargetParticipant->Owner->GetIndex())
				{
					CurrentMatch->GetParticipant(Player, LosingParticipant);
					LosingParticipant->Profits -= (Player->GetPotContribution() - ContriTillNow) < ContriNeeded ? (Player->GetPotContribution() - ContriTillNow) : ContriNeeded;
					LosingParticipant->MoneyLost += (Player->GetPotContribution() - ContriTillNow) < ContriNeeded ? (Player->GetPotContribution() - ContriTillNow) : ContriNeeded;
				}
			}
		}
		//Multiple winners
		else
		{
			unsigned int Portion = Pots[PotIndex] / PotWinners.size();

			for (auto const Winner : PotWinners)
			{
				AwardPlayer(Winner, Portion);

				CurrentMatch->GetParticipant(Winner, TargetParticipant);
				TargetParticipant->Profits += Portion;
				TargetParticipant->MoneyWon += Portion;
			}

			for (auto const Player : ValidPlayersPerPot[PotIndex])
			{
				if (std::find(PotWinners.begin(), PotWinners.end(), Player) == PotWinners.end())
				{
					CurrentMatch->GetParticipant(Player, LosingParticipant);
					LosingParticipant->Profits -= (Player->GetPotContribution() - ContriTillNow) < ContriNeeded ? (Player->GetPotContribution() - ContriTillNow) : ContriNeeded;
					LosingParticipant->MoneyLost += (Player->GetPotContribution() - ContriTillNow) < ContriNeeded ? (Player->GetPotContribution() - ContriTillNow) : ContriNeeded;
				}
			}
		}

		ContriTillNow += Pots[PotIndex] / ValidPlayersPerPot[PotIndex].size();
	}

	//Update Hand Win/Loss 
	int HighestProfit = 0;

	for (auto const Participant : CurrentMatch->RankingBoard)
	{
		if (!Participant->Owner->GetIsBroke() && !Participant->Owner->GetIsFolded() && Participant->Profits > HighestProfit)
		{
			HighestProfit = Participant->Profits;
			TargetParticipant = Participant;
		}
	}

	for (auto const Participant : CurrentMatch->RankingBoard)
	{
		if (!Participant->Owner->GetIsBroke() && !Participant->Owner->GetIsFolded())
		{
			if (Participant->Profits < HighestProfit)
				Participant->HandsLost += 1;
			else
				Participant->HandsWon += 1;
		}
	}

	for (auto const& Participant : CurrentMatch->RankingBoard)
		Participant->Profits = 0;*/

	/*std::vector<unsigned int> Pots;
	std::vector<std::vector<std::shared_ptr<Player>>> ValidPlayersPerPot;

	SplitPot(Pots, ValidPlayersPerPot);

	std::vector<std::shared_ptr<Player>> PotWinners;
	std::shared_ptr<Participant> TargetParticipant;

	unsigned int PotTillNow = 0;

	for (auto const Participant : CurrentMatch->RankingBoard)
	{
		if (!Participant->Owner->GetIsBroke() && Participant->Owner->GetIsFolded())
		{
			Participant->MoneyLost += Participant->Owner->GetPotContribution();
			Participant->HandsLost += 1;
		}
	}

	for (unsigned int PotIndex = 0; PotIndex < Pots.size(); PotIndex++)
	{
		if (ValidPlayersPerPot[PotIndex].size() == 1)
		{
			AwardPlayer(ValidPlayersPerPot[PotIndex][0], Pots[PotIndex]);
			
			if(PrintProcess)
				std::cout << "P." << ValidPlayersPerPot[PotIndex][0]->GetIndex() << " solely win $" << Pots[PotIndex] << " from Pot " << PotIndex + 1 << " (Stack: $" << ValidPlayersPerPot[PotIndex][0]->GetStack() << ") \n";

			CurrentMatch->GetParticipant(ValidPlayersPerPot[PotIndex][0], TargetParticipant);
			TargetParticipant->MoneyWon += Pots[PotIndex];
			TargetParticipant->HandsWon += 1;

			for (auto const Participant : CurrentMatch->RankingBoard)
			{
				if (!Participant->Owner->GetIsBroke() && !Participant->Owner->GetIsFolded() && Participant->Owner->GetIsContributing() && Participant->Owner->GetIndex() != TargetParticipant->Owner->GetIndex())
				{
					Participant->MoneyLost += (Participant->Owner->GetPotContribution() - PotTillNow) < Pots[PotIndex] ? (Participant->Owner->GetPotContribution() - PotTillNow) : Pots[PotIndex];
					Participant->HandsLost += 1;	
				}
			}

			continue;
		}

		DetermineWinningPlayers(ValidPlayersPerPot[PotIndex], PotWinners);

		if (PotWinners.size() == 1)
		{
			AwardPlayer(PotWinners[0], Pots[PotIndex]);

			if(PrintProcess)
				std::cout << "P." << PotWinners[0]->GetIndex() << " win $" << Pots[PotIndex] << " from Pot " << PotIndex + 1 << " (Stack: $" << PotWinners[0]->GetStack() << ") \n";
		
			CurrentMatch->GetParticipant(PotWinners[0], TargetParticipant);
			TargetParticipant->MoneyWon += Pots[PotIndex];
			TargetParticipant->HandsWon += 1;

			for (auto const Participant : CurrentMatch->RankingBoard)
			{
				if (!Participant->Owner->GetIsBroke() && !Participant->Owner->GetIsFolded() && Participant->Owner->GetIsContributing() &&  Participant->Owner->GetIndex() != TargetParticipant->Owner->GetIndex() && Participant->Owner->GetPotContribution() > PotTillNow)
				{
					Participant->MoneyLost += (Participant->Owner->GetPotContribution() - PotTillNow) < Pots[PotIndex] ? (Participant->Owner->GetPotContribution() - PotTillNow) : Pots[PotIndex];
					Participant->HandsLost += 1;

					if (PrintProcess)
						std::cout << "P." << Participant->Owner->GetIndex() << " lose $" << Participant->Owner->GetPotContribution() << " (Stack: $" << Participant->Owner->GetStack() << ")\n";
				}
			}
		}

		else if (PotWinners.size() > 1)
		{
			unsigned int Portion = Pots[PotIndex] / PotWinners.size();

			for (unsigned int WinnerIndex = 0; WinnerIndex < PotWinners.size(); WinnerIndex++)
			{
				AwardPlayer(PotWinners[WinnerIndex], Portion);
				
				if(PrintProcess)
					std::cout << "P." << PotWinners[WinnerIndex]->GetIndex() << " win $" << Portion << " from Pot " << PotIndex + 1 << " (Stack: $" << PotWinners[WinnerIndex]->GetStack() << ") \n";
			
				CurrentMatch->GetParticipant(PotWinners[WinnerIndex], TargetParticipant);
				TargetParticipant->MoneyWon += Portion;
				TargetParticipant->HandsWon += 1;
			}

			for (auto const Participant : CurrentMatch->RankingBoard)
			{
				if (!Participant->Owner->GetIsBroke() && !Participant->Owner->GetIsFolded() && Participant->Owner->GetIsContributing() && std::find(PotWinners.begin(), PotWinners.end(), Participant->Owner) == PotWinners.end() && Participant->Owner->GetPotContribution() > PotTillNow)
				{
					Participant->MoneyLost += (Participant->Owner->GetPotContribution() - PotTillNow) < Pots[PotIndex] ? (Participant->Owner->GetPotContribution() - PotTillNow) : Pots[PotIndex];
					Participant->HandsLost += 1;

					if (PrintProcess)
						std::cout << "P." << Participant->Owner->GetIndex() << " lose $" << Participant->Owner->GetPotContribution() << " (Stack: $" << Participant->Owner->GetStack() << ")\n";
				}
			}
		}

		PotTillNow += Pots[PotIndex];
		PotWinners.clear();
	}*/

	ClearCommunalCards();
	EmptyPot();

	for (auto const& Player : Players)
	{
		Player->EmptyHand();
		Player->SetAnte(0);
		Player->SetIsFolded(false);

		if (Player->GetStack() <= 0 && !Player->GetIsBroke())
		{
			Player->SetIsBroke(true);

			//if (PrintProcess)
				std::cout << "P." << Player->GetIndex() << " is broke...\n";
		}
	}

	if (IsGameEnded())
	{
		IsActive = false;
		return;
	}

	if (PrintProcess)
	{
		/*std::cout << "Result: / ";
		for (unsigned int Index = 0; Index < Players.size(); Index++)
			std::cout << "P." << Players[Index]->GetIndex() << ": " << Players[Index]->GetStack() << " ";*/


		std::cout << "------------------------------------------------------------------------------------\n";
		std::cout << "\n";
	}

	StartRound();
}

void Table::StartPhase()
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
		std::cout << "Table: " << Evaluator->GetStr(CommunalCards) << "\n";	
	}
}

void Table::UpdatePhase()
{
	//std::cout << "enter update phase for Player " << CurrentPlayer->GetIndex() << "\n";
	//std::cout << "testing for round/phase end...\n";
	if (IsPhaseEnded() && CurrentState == Phase::River)
	{
		//std::cout << "Round has ended... (Phase: " << GetStateStr() << ")\n";
		EndRound();

		return;
	}
	else if (IsPhaseEnded() && CurrentState != Phase::River)
	{
		NextPhase();
		//std::cout << "Moving to next phase...(" << GetStateStr() << ")\n";
		return;
		
	}
	else if (CurrentPlayer->GetIsBroke() || CurrentPlayer->GetIsFolded() || !CurrentPlayer->GetIsContributing())
	{
		//std::cout << "Player " << CurrentPlayer->GetIndex() << " is not valid, Getting next player...\n";
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
				CurrentPlayer->SetIsContributing(false);
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
				CurrentPlayer->SetIsContributing(false);
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
				CurrentPlayer->SetIsContributing(false);
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

	CurrentPlayer = GetNextPlayer(CurrentPlayer);
	//std::cout << "moving to next player, P." << CurrentPlayer->GetIndex() << "\n";
}

void Table::NextPhase()
{
	CurrentState = static_cast<Phase>(static_cast<int>(CurrentState) + 1);
	StartPhase();
}

bool Table::IsPhaseEnded()
{
	for (auto const& Player: Players)
	{
		if (!Player->GetIsBroke() && !Player->GetIsFolded() && Player->GetIsContributing())
		{
			if (CurrentState == Phase::Preflop && Player->GetAnte() < RequiredAnte)
				return false;

			else if (CurrentState != Phase::Preflop && (Player->GetAction() == BettingAction::NONE || Player->GetAnte() < RequiredAnte))
				return false;
		}
	}

	return true;
}

bool Table::IsRoundEnded()
{
	unsigned int RemainingPlayers = 0;

	for (auto const& Player : Players)
		if (!Player->GetIsFolded() && !Player->GetIsBroke()) RemainingPlayers++;

	return RemainingPlayers <= 1 ? true : false;
}

bool Table::IsGameEnded()
{
	unsigned int Count = 0;
	
	for (unsigned int Index = 0; Index < Players.size(); Index++)
		if (!Players[Index]->GetIsBroke()) Count++;
	
	return Count <= 1 ? true : false;
}

void Table::AddPlayer(std::shared_ptr<Player> _AddingPlayer)
{
	Players.push_back(_AddingPlayer);
	//Earnings.push_back(0);
	//Records.insert(std::make_pair(_AddingPlayer, 0));

	_AddingPlayer->SetStack(EntryStack);
}

void Table::AddPlayer(std::shared_ptr<Player> _AddingPlayer, unsigned int _EntryStack)
{
	Players.push_back(_AddingPlayer);
	//Earnings.push_back(0);
	//Records.insert(std::make_pair(_AddingPlayer, 0));

	_AddingPlayer->SetStack(_EntryStack);
}

void Table::RemoveAllPlayers()
{
	Players.clear();
}

std::shared_ptr<Player> Table::GetPreviousPlayer(const std::shared_ptr<Player>& _Reference)
{
	std::vector<std::shared_ptr<Player>> ActivePlayers;
	GetParticipatingPlayers(ActivePlayers);

	if (ActivePlayers.size() == 0 || (ActivePlayers.size() == 1 && ActivePlayers[0]->GetIndex() == _Reference->GetIndex()))
		return nullptr;

	if (std::find(ActivePlayers.begin(), ActivePlayers.end(), _Reference) == ActivePlayers.end())
	{
		auto RefItr = std::find(Players.begin(), Players.end(), _Reference);

		if (RefItr == Players.end())
			return nullptr;

		for (auto Itr = RefItr == Players.begin() ? std::prev(Players.end()) : std::prev(RefItr);
			Itr != Players.begin(); Itr--)
		{
			if ((*Itr)->GetIndex() == (*RefItr)->GetIndex())
				continue;

			if (std::find(ActivePlayers.begin(), ActivePlayers.end(), (*Itr)) != ActivePlayers.end())
				return *Itr;

			if (std::prev(Itr) == Players.begin())
				Itr = std::prev(Players.end());
		}
	}

	for (unsigned int Index = 0; Index < ActivePlayers.size(); Index++)
	{
		if (ActivePlayers[Index] == _Reference)
			return Index <= 0 ? ActivePlayers[ActivePlayers.size() - 1] : ActivePlayers[Index - 1];
	}

	std::cout << "Prev player cannot be found...\n";
	return nullptr;


	/*for (unsigned int Index = 0; Index < Players.size(); Index++)
	{
		if (Players[Index] == _Reference)
			return Index <= 0 ? Players[Players.size() - 1] : Players[Index - 1];
	}

	return nullptr;*/
}

std::shared_ptr<Player> Table::GetNextPlayer(const std::shared_ptr<Player>& _Reference)
{
	std::vector<std::shared_ptr<Player>> ActivePlayers;
	GetParticipatingPlayers(ActivePlayers);

	if (ActivePlayers.size() == 0)// || (ActivePlayers.size() == 1 && ActivePlayers[0]->GetIndex() == _Reference->GetIndex()))
		return nullptr;
	
	auto RefItr = std::find(ActivePlayers.begin(), ActivePlayers.end(), _Reference);

	if (RefItr == ActivePlayers.end())
	{
		RefItr = std::find(Players.begin(), Players.end(), _Reference);
		
		if (RefItr == Players.end())
			return nullptr;
	
		for (unsigned int Index = 0; Index < Players.size(); Index++)
		{
			RefItr = (*RefItr)->GetIndex() == (*std::prev(Players.end()))->GetIndex() ? Players.begin() : std::next(RefItr);

			if (std::find(ActivePlayers.begin(), ActivePlayers.end(), (*RefItr)) != ActivePlayers.end())
				return (*RefItr);
		}
	}
	else
	{
		RefItr = RefItr == std::prev(ActivePlayers.end()) ? ActivePlayers.begin() : std::next(RefItr);
		return (*RefItr);
	}

	std::cout << "Next player cannot be found...\n";
	return nullptr;

	/*for (unsigned int Index = 0; Index < Players.size(); Index++)
	{
		if (Players[Index] == _Reference)
			return Index >= (Players.size() - 1) ? Players[0] : Players[Index + 1];
	}

	return nullptr;*/
}

void Table::UpdatePot()
{
	EmptyPot();

	for (auto const& Player : Players)
		Pot += Player->GetPotContribution();
}

void Table::EmptyPot()
{
	Pot = 0;
}

void Table::SplitPot(std::vector<unsigned int> &_Pots, std::vector<std::vector<std::shared_ptr<Player>>>& _ContributorsPerPot, std::vector<std::vector<std::shared_ptr<Player>>>& _ContestantsPerPot)
{
	std::vector<std::shared_ptr<Player>> CurrentPlayers;
	for (auto const& Player : Players)
	{
		if (!Player->GetIsBroke())
			CurrentPlayers.push_back(Player);
	}

	std::sort(CurrentPlayers.begin(), CurrentPlayers.end(),
		[](std::shared_ptr<Player> _First, std::shared_ptr<Player> _Second) { return _First->GetPotContribution() > _Second->GetPotContribution(); });

	std::vector<unsigned int> PotsRequirement;

	PotsRequirement.push_back(CurrentPlayers[0]->GetPotContribution());
	for (auto const & Player : CurrentPlayers)
	{
		if (!Player->GetIsFolded() && !Player->GetIsContributing() && Player->GetPotContribution() > 0 && std::find(PotsRequirement.begin(), PotsRequirement.end(), Player->GetPotContribution()) == PotsRequirement.end())
			PotsRequirement.push_back(Player->GetPotContribution());
	}

	unsigned int RequirementTillNow = 0;

	for (unsigned int Requirement : PotsRequirement)
	{
		std::vector<std::shared_ptr<Player>> Contributors, Contestants;

		RequirementTillNow += Requirement;

		for (auto const& Player : CurrentPlayers)
		{
			if (!Player->GetIsBroke())
			{
				//std::cout << "P." << Player->GetIndex() << ": Contributing (" << Player->GetIsContributing() << ") w/" << Player->GetPotContribution() << "\n";

 				if ((Player->GetIsContributing() && Player->GetPotContribution() >= RequirementTillNow) || (!Player->GetIsContributing() && Player->GetPotContribution() > 0))
					Contributors.push_back(Player);

				if (!Player->GetIsFolded())
					Contestants.push_back(Player);
			}
		}

		_Pots.push_back(Requirement * Contributors.size());
		_ContributorsPerPot.push_back(Contributors);
		_ContestantsPerPot.push_back(Contestants);
	}

	/*std::vector<std::shared_ptr<Player>> CurrentPlayers;
	for (auto const& Player : Players)
	{
		if (!Player->GetIsBroke())
			CurrentPlayers.push_back(Player);
	}
	
	std::sort(CurrentPlayers.begin(), CurrentPlayers.end(),
		[](std::shared_ptr<Player> _First, std::shared_ptr<Player> _Second) { return _First->GetPotContribution() > _Second->GetPotContribution(); });

	_Pots.push_back(CurrentPlayers[0]->GetPotContribution());
	for (auto const & Player : CurrentPlayers)
	{
		if (!Player->GetIsFolded() && !Player->GetIsContributing() && Player->GetPotContribution() > 0 && std::find(_Pots.begin(), _Pots.end(), Player->GetPotContribution()) == _Pots.end())
			_Pots.push_back(Player->GetPotContribution());
	}

	unsigned int PotTillNow = 0;

	for (unsigned int Pot : _Pots)
	{
		std::vector<std::shared_ptr<Player>> Contributors;
		std::vector<std::shared_ptr<Player>> Contestants;

		PotTillNow += Pot;

		for (auto const& Player : CurrentPlayers)
		{
			if (!Player->GetIsBroke())
			{
				if((Player->GetIsContributing() && Player->GetPotContribution() >= PotTillNow) || (!Player->GetIsContributing() && Player->GetPotContribution() > 0))
					Contributors.push_back(Player);

				if (!Player->GetIsFolded())
					Contestants.push_back(Player);
			}
		}

		_ContributorsPerPot.push_back(Contributors);
		_ContestantsPerPot.push_back(Contestants);
	}*/
}

void Table::DistributeWinnings()
{
	std::vector<std::shared_ptr<Participant>> Participants;
	std::cout << "Pot Contri:\n";
	for (auto const& Participant : CurrentMatch->RankingBoard)
	{
		std::cout << "P." << Participant->Owner->GetIndex() << ": " << Participant->Owner->GetPotContribution() << "\n";

		if(Participant->Owner->GetPotContribution() > 0)
			Participants.push_back(Participant);
	}

	for (auto const& Participant : Participants)
		Participant->Profits = -Participant->Owner->GetPotContribution();

	unsigned int MinStack = 0, CurrentPot = 0;

	while (Participants.size() > 1)
	{
		MinStack = Participants[0]->Owner->GetPotContribution();
		for (auto const& Participant : Participants)
			MinStack = Participant->Owner->GetPotContribution() < MinStack ? Participant->Owner->GetPotContribution() : MinStack;

		CurrentPot += MinStack * Participants.size();

		for (auto const& Participant : Participants)
			Participant->Owner->SetPotContribution(Participant->Owner->GetPotContribution() - MinStack);

		std::vector<std::shared_ptr<Participant>> Winners;
		DetermineWinningPlayers(Participants, Winners);

		for (auto const& Winner : Winners)
		{
			Winner->Profits += CurrentPot / Winners.size();
			AwardPlayer(Winner->Owner, (CurrentPot / Winners.size()));
			std::cout << "P." << Winner->Owner->GetIndex() << " won " << (CurrentPot / Winners.size()) << "\n";
		}

		auto Itr = std::begin(Participants);

		while(Itr != Participants.end())
		{
			if ((*Itr)->Owner->GetPotContribution() <= 0)
				Itr = Participants.erase(Itr);
			else
				++Itr;
		}

		if (Participants.size() <= 1 && CurrentPot == 0)
			std::cout << "\n";

		CurrentPot = 0;
	}

	if (Participants.size() == 1)
	{
		Participants[0]->Profits += Participants[0]->Owner->GetPotContribution();
		AwardPlayer(Participants[0]->Owner, Participants[0]->Owner->GetPotContribution());
		std::cout << "P." << Participants[0]->Owner->GetIndex() << " get back contribution leftover of " << Participants[0]->Owner->GetPotContribution() << "\n";
	}
}

void Table::DealCardsToPlayers()
{
	if (!CurrentMatch->IsDuplicated)
	{
		ActiveDeck->Refill();
		ActiveDeck->Shuffle();
	}
	else if (CurrentMatch->IsDuplicated && CurrentMatch->Index == 0)
	{
		ActiveDeck->Refill();
		ActiveDeck->Shuffle();

		ArrangedDeck->CopyFrom(ActiveDeck);
	}
	else if (CurrentMatch->IsDuplicated && CurrentMatch->Index > 0)
	{
		ActiveDeck->CopyFrom(ArrangedDeck);
	}

	std::vector<std::shared_ptr<Player>> ActivePlayers;
	std::shared_ptr<Player> RefPlayer = CurrentPlayer;
	GetParticipatingPlayers(ActivePlayers);

	for (unsigned int Index = 0; Index < ActivePlayers.size(); Index++)
	{
		RefPlayer->EmptyHand();
		RefPlayer->SetHand(ActiveDeck->Draw(), ActiveDeck->Draw());

		RefPlayer = GetNextPlayer(RefPlayer);
	}

	//for (auto const& Player : Players)
	//{
	//	Player->EmptyHand();
	//	Player->SetHand(ActiveDeck->Draw(), ActiveDeck->Draw());
	//	
	//	//std::cout << "P." << Player->GetIndex() << " drew " << Player->GetHandInfo() << "\n";
	//}
}

void Table::IssueCommunalCards()
{
	switch(CurrentState)
	{
		case Phase::Flop:
		{
			for (unsigned int Index = 0; Index < 3; Index++)
				CommunalCards[Index] = ActiveDeck->Draw();
			break;
		}
		case Phase::Turn:
		{
			CommunalCards[3] = ActiveDeck->Draw();
			break;
		}
		case Phase::River:
		{
			CommunalCards[4] = ActiveDeck->Draw();
			break;
		}
		default:
		{
			std::cout << "Communal Cards cannot be issued in this state." << std::endl;
			break;
		}
	}
}

void Table::ClearCommunalCards()
{
	for (auto& Card : CommunalCards)
		Card = nullptr;
}

void Table::ShiftDealer(const std::shared_ptr<Player>& _Target)
{
	DealingPlayer = _Target;
	SmallBlindPlayer = GetNextPlayer(_Target);
	BigBlindPlayer = GetNextPlayer(SmallBlindPlayer);
	CurrentPlayer = GetNextPlayer(BigBlindPlayer);
}

unsigned int Table::GetEntryIndex(const std::shared_ptr<Player>& _Target)
{
	for (unsigned int Index = 0; Index < Players.size(); Index++)
	{
		if (Players[Index] == _Target)
			return Index;
	}

	std::cout << "PLAYER ENTRY INDEX CANNOT BE FOUND FOR P." << _Target->GetIndex() << std::endl;
	return Players.size();
}

int Table::GetPlayerEarnings(const std::shared_ptr<Player>& _Target)
{
	return 0;//Earnings[GetEntryIndex(_Target)];
}

void Table::GetParticipatingPlayers(std::vector<std::shared_ptr<Player>>& _Participants)
{
	for (auto const& Player : Players)
	{
		if (!Player->GetIsBroke() && !Player->GetIsFolded() && Player->GetIsContributing()) 
			_Participants.push_back(Player);
	}
}

void Table::DetermineWinningPlayers(const std::vector<std::shared_ptr<Player>>& _Contestants, std::vector<std::shared_ptr<Player>>& _Winners)
{
	if (_Contestants.size() == 1)
	{
		_Winners = _Contestants;
		return;
	}

	std::vector<std::array<std::shared_ptr<Card>,5>> BettingHands;

	for (auto const& Contestant : _Contestants)
		BettingHands.push_back(Evaluator->GetBestCommunalHand(Contestant->GetHand(), CommunalCards));

	std::array<std::shared_ptr<Card>,5> BestHand = BettingHands[0];

	_Winners.push_back(_Contestants[0]);

	for (unsigned int Index = 1; Index < BettingHands.size(); Index++)
	{
		switch (Evaluator->IsBetter5Cards(BettingHands[Index],BestHand))
		{
			case ComparisonResult::Win:
			{
				BestHand = BettingHands[Index];

				_Winners.clear();
				_Winners.push_back(_Contestants[Index]);
				break;
			}
			case ComparisonResult::Draw:
			{
				_Winners.push_back(_Contestants[Index]);
				break;
			}
			default:
				break;
		}
	}
}

void Table::DetermineWinningPlayers(std::vector<std::shared_ptr<Participant>>& _Participants, std::vector<std::shared_ptr<Participant>>& _Winners)
{
	if (_Participants.size() == 1)
	{
		_Winners = _Participants;
		return;
	}

	auto Itr = _Participants.begin();
	while (Itr != _Participants.end())
	{
		if ((*Itr)->Owner->GetIsBroke() || (*Itr)->Owner->GetIsFolded())
			Itr = _Participants.erase(Itr);
		else
			++Itr;
	}

	std::vector<std::array<std::shared_ptr<Card>, 5>> BettingHands;

	for (auto const& Participant : _Participants)
		BettingHands.push_back(Evaluator->GetBestCommunalHand(Participant->Owner->GetHand(), CommunalCards));

	std::array<std::shared_ptr<Card>, 5> BestHand = BettingHands[0];

	_Winners.push_back(_Participants[0]);

	for (unsigned int Index = 1; Index < BettingHands.size(); Index++)
	{
		switch (Evaluator->IsBetter5Cards(BettingHands[Index], BestHand))
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

void Table::AwardPlayer(const std::shared_ptr<Player>& _Player, unsigned int _Amt)
{
	_Player->SetStack(_Player->GetStack() + _Amt);
}

void Table::RankPlayers(std::vector<int>& _Rankings)
{
	//_Rankings.push_back
}

void Table::SetMatch(const std::shared_ptr<Match>& _NewMatch)
{
	CurrentMatch = _NewMatch;

	/*for (auto const Participant : CurrentMatch->RankingBoard)
		AddPlayer(Participant->Owner);*/
}

void Table::Print()
{
	std::cout << "\n--------------------------------------------------\n";
	
	std::cout << "Table: ";
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
		else if (!Players[Index]->GetIsFolded() && !Players[Index]->GetIsContributing())
			std::cout << "P." << Index << " ($" << Players[Index]->GetStack() << "): " << ": All-in \n" << "   |   $" << Players[Index]->GetAnte() << "\n";
		else
			std::cout << "P." << Index << " ($" << Players[Index]->GetStack() << "): " << Players[Index]->GetHandCardByIndex(0)->GetInfo() << "," << Players[Index]->GetHand()[1]->GetInfo() << "  |  $" << Players[Index]->GetAnte() << "\n";
	}

	std::cout << "--------------------------------------------------\n\n";
}

std::string Table::GetStateStr()
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
