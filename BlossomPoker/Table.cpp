#include "Table.h"
#include "Card.h"
#include "Player.h"
#include "HandEvaluator.h"

#include <algorithm>
#include <iterator>

Table::Table(std::shared_ptr<HandEvaluator> _Evaluator, unsigned int _BigBlind, bool _PrintProcess)
	: SmallBlind(_BigBlind / 2), BigBlind(_BigBlind), RequiredAnte(BigBlind), EntryStack(100 * _BigBlind), Pot(0), Round(1),
	Evaluator(_Evaluator),
	PrintProcess(_PrintProcess)
{};

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

	CommunalCards.clear();

	ActiveDeck.Refill();
	ActiveDeck.Shuffle();

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
	//std::cout << "Round " << Round << ":\n";
	DistributeWinnings();

	int HighestProfit = CurrentMatch->GetParticipant(0)->GetProfits();
	for (auto const& Participant : CurrentMatch->GetRankingBoard()) 
	{
		if (!Participant->GetOwner()->GetIsBroke() && !Participant->GetOwner()->GetIsFolded() && Participant->GetProfits() > HighestProfit)
			HighestProfit = Participant->GetProfits();
	}

	if (HighestProfit > 0)
	{
		for (auto const& Participant : CurrentMatch->GetRankingBoard())
		{
			if (Participant->GetProfits() == HighestProfit)
			{
				Participant->SetMoneyWon(Participant->GetMoneyWon() + Participant->GetProfits());
				Participant->SetHandsWon(Participant->GetHandsWon() + 1);

				/*CurrentMatch->PrintInfo();
				std::cout << "\n";
				std::cout << "P." << Participant->GetOwner()->GetIndex() << " won this hand...\n";
				std::cout << "\n";
				CurrentMatch->PrintInfo();
				std::cout << "\n";*/
			}
		}

		for (auto const& Participant : CurrentMatch->GetRankingBoard())
		{
			if (!Participant->GetOwner()->GetIsBroke() && Participant->GetProfits() < HighestProfit)
			{
				Participant->SetHandsLost(Participant->GetHandsLost() + 1);

				if (Participant->GetProfits() > 0)
					Participant->SetMoneyWon(Participant->GetMoneyWon() + Participant->GetProfits());
				else
					Participant->SetMoneyLost(Participant->GetMoneyLost() - Participant->GetProfits());

				/*CurrentMatch->PrintInfo();
				std::cout << "\n";
				std::cout << "Player " << Participant->GetOwner()->GetIndex() << " lost this hand...\n";
				std::cout << "\n";
				CurrentMatch->PrintInfo();
				std::cout << "\n";*/
			}
		}
	}
	else if (HighestProfit == 0)
	{
		for (auto const& Participant : CurrentMatch->GetRankingBoard())
		{
			/*CurrentMatch->PrintInfo();
			std::cout << "\n";*/

			if (!Participant->GetOwner()->GetIsBroke() && !Participant->GetOwner()->GetIsFolded())
			{
				Participant->SetHandsWon(Participant->GetHandsWon() + 1);
				//std::cout << "P." << Participant->GetOwner()->GetIndex() << " won the hand...\n";
			}

			else if (!Participant->GetOwner()->GetIsBroke() && Participant->GetOwner()->GetIsFolded())
			{
				Participant->SetHandsLost(Participant->GetHandsLost() + 1);
				//std::cout << "P." << Participant->GetOwner()->GetIndex() << " lose the hand...\n";
			}

			/*std::cout << "\n";
			CurrentMatch->PrintInfo();
			std::cout << "\n";*/
		}
	}
	

	for (auto const& Participant : CurrentMatch->GetRankingBoard())
		Participant->SetProfits(0);

	CommunalCards.clear();
	EmptyPot();

	for (auto const& Player : Players)
	{
		Player->SetAnte(0);
		Player->SetIsFolded(false);

		if (Player->GetStack() <= 0 && !Player->GetIsBroke())
		{
			Player->SetIsBroke(true);

			if (PrintProcess)
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
		{
			if (Player->GetIsBroke())
				std::cout << "P." << Player->GetIndex() << ": BROKE  ";
			else if (Player->GetIsFolded())
				std::cout << "P." << Player->GetIndex() << ": FOLD  ";
			else
				std::cout << "P." << Player->GetIndex() << ": " << Evaluator->GetStr(Player->GetHand()) << "  ";
		}

		std::cout << "\n";
		std::cout << "Table: " << Evaluator->GetStr(CommunalCards) << "\n";	
	}
}

void Table::UpdatePhase()
{
	//std::cout << "Updating P." << CurrentPlayer->GetIndex() << "...\n";
	
	if (IsRoundEnded() || (IsPhaseEnded() && CurrentState == Phase::River))
	{
		EndRound();
		//std::cout << "Round has ended... (Final phase: " << GetStateStr() << ")\n";
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
		//std::cout << "P." << CurrentPlayer->GetIndex() << " is not valid, Getting next player...\n";
		CurrentPlayer = GetNextPlayer(CurrentPlayer);
		return;
	}
	
	UpdatePot();
	CurrentPlayer->Update();

	switch(CurrentPlayer->GetAction())
	{
		case BettingAction::Fold:
		{
			CurrentPlayer->SetIsFolded(true);
		
			if (PrintProcess) 
				std::cout << "P." << CurrentPlayer->GetIndex() << " folded. \n";
			
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
			CurrentPlayer->SetAnte(RequiredAnte);
			UpdatePot();

			if (PrintProcess)
				std::cout << "P." << CurrentPlayer->GetIndex() << " called to $" << RequiredAnte << " results in Pot to be $" << Pot << "\n";
			
			if (CurrentPlayer->GetStack() <= 0)
			{
				CurrentPlayer->SetIsContributing(false);
				//std::cout << "P." << CurrentPlayer->GetIndex() << " is no longer participating...\n";
			}
			
			break;
		}
		case BettingAction::Raise:
		{
			unsigned int RaiseAmt = CurrentPlayer->GetAI().GetRaiseBetAmt();
			CurrentPlayer->SetAnte(CurrentPlayer->GetAnte() + RaiseAmt);
			RequiredAnte = CurrentPlayer->GetAnte();
			UpdatePot();

			if (PrintProcess)
				std::cout << "P." << CurrentPlayer->GetIndex() << " raised Pot to $" << Pot <<"\n";

			if (CurrentPlayer->GetStack() <= 0)
			{
				CurrentPlayer->SetIsContributing(false);
				//std::cout << "P." << CurrentPlayer->GetIndex() << " is no longer participating...\n";
			}
		
			break;
		}
		case BettingAction::Bet:
		{
			unsigned int BetAmt = CurrentPlayer->GetAI().GetRaiseBetAmt();
			CurrentPlayer->SetAnte(CurrentPlayer->GetAnte() + BetAmt);
			RequiredAnte = CurrentPlayer->GetAnte();
			UpdatePot();

			if (PrintProcess)
				std::cout << "P." << CurrentPlayer->GetIndex() << " bet $" << BetAmt << " results in Pot to $" << Pot << " \n";

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

		_Pots.push_back(Requirement * (unsigned int) Contributors.size());
		_ContributorsPerPot.push_back(Contributors);
		_ContestantsPerPot.push_back(Contestants);
	}
}

void Table::DistributeWinnings()
{
	std::vector<std::shared_ptr<Participant>> Participants;
	Participants.reserve(CurrentMatch->GetRankingBoard().size());

	//std::cout << "Pot Contri (1) :\n";
	for (auto const& Participant : CurrentMatch->GetRankingBoard())
	{
		if(Participant->GetOwner()->GetPotContribution() > 0)
			Participants.push_back(Participant);

		//std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetOwner()->GetPotContribution() << "\n";
	}

	//std::cout << "Initial Profit:\n";
	for (auto const& Participant : Participants)
	{
		Participant->SetProfits(-Participant->GetOwner()->GetPotContribution());

		//std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetProfits() << "\n";
	}

	unsigned int MinStack = 0, CurrentPot = 0;

	while (Participants.size() > 1)
	{
		MinStack = Participants[0]->GetOwner()->GetPotContribution();
		for (auto const& Participant : Participants)
			MinStack = Participant->GetOwner()->GetPotContribution() < MinStack ? Participant->GetOwner()->GetPotContribution() : MinStack;

		//std::cout << "MinStack: " << MinStack << "\n";
		//std::cout << "Participants: " << Participants.size() << "\n";

		CurrentPot += MinStack * (unsigned int) Participants.size();

		//std::cout << "CurrentPot: " << CurrentPot << "\n";

		//std::cout << "Pot Contri (2) :\n";
		for (auto const& Participant : Participants)
		{
			Participant->GetOwner()->SetPotContribution(Participant->GetOwner()->GetPotContribution() - MinStack);
			//std::cout << "P." << Participant->GetOwner()->GetIndex() << ": " << Participant->GetOwner()->GetPotContribution() << "\n";
		}

		std::vector<std::shared_ptr<Participant>> Winners;
		DetermineWinningPlayers(Participants, Winners);

 		for (auto const& Winner : Winners)
		{
			Winner->SetProfits(Winner->GetProfits() + (CurrentPot / (unsigned int) Winners.size()));
			AwardPlayer(Winner->GetOwner(), (CurrentPot / (unsigned int) Winners.size()));

			//std::cout << "P." << Winner->GetOwner()->GetIndex() << " won " << (CurrentPot / Winners.size()) << "\n";
		}

		auto Itr = std::begin(Participants);

		while(Itr != Participants.end())
		{
			if ((*Itr)->GetOwner()->GetPotContribution() <= 0)
				Itr = Participants.erase(Itr);
			else
				++Itr;
		}

		CurrentPot = 0;
	}

	if (Participants.size() == 1)
	{
		Participants[0]->SetProfits(Participants[0]->GetProfits() + Participants[0]->GetOwner()->GetPotContribution());
		AwardPlayer(Participants[0]->GetOwner(), Participants[0]->GetOwner()->GetPotContribution());

		//std::cout << "P." << Participants[0]->GetOwner()->GetIndex() << " get back contribution leftover of " << Participants[0]->GetOwner()->GetPotContribution() << "\n";
	}

	//std::cout << "\n";
}

void Table::RestockDeck()
{
	ActiveDeck.Refill();
	ActiveDeck.Shuffle();
}

void Table::SaveDeckArrangement()
{
	ArrangedDeck.CopyFrom(ActiveDeck);
}

void Table::LoadDeckArrangement()
{
	ActiveDeck.CopyFrom(ArrangedDeck);
}

void Table::DealCardsToPlayers()
{
	if (Round > 0)
		RestockDeck();

	std::vector<std::shared_ptr<Player>> ActivePlayers;
	std::shared_ptr<Player> RefPlayer = CurrentPlayer;
	GetParticipatingPlayers(ActivePlayers);

	for (unsigned int Index = 0; Index < ActivePlayers.size(); Index++)
	{
		RefPlayer->SetHand(ActiveDeck.Draw(), ActiveDeck.Draw());
		RefPlayer = GetNextPlayer(RefPlayer);
	}
}

void Table::IssueCommunalCards()
{
	switch(CurrentState)
	{
		case Phase::Flop:
		{
			CommunalCards.clear();
			for (unsigned int Index = 0; Index < 3; Index++)
				CommunalCards.push_back(ActiveDeck.Draw());
			break;
		}
		case Phase::Turn:
		{
			CommunalCards.push_back(ActiveDeck.Draw());
			break;
		}
		case Phase::River:
		{
			CommunalCards.push_back(ActiveDeck.Draw());
			break;
		}
		default:
		{
			std::cout << "Communal Cards cannot be issued in this state." << std::endl;
			break;
		}
	}
}

void Table::ShiftDealer(const std::shared_ptr<Player>& _Target)
{
	DealingPlayer = _Target;
	SmallBlindPlayer = GetNextPlayer(_Target);
	BigBlindPlayer = GetNextPlayer(SmallBlindPlayer);
	CurrentPlayer = GetNextPlayer(BigBlindPlayer);
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

	std::vector<std::array<Card,5>> BettingHands;

	for (auto const& Contestant : _Contestants)
		BettingHands.push_back(Evaluator->GetBestCommunalHand(Contestant->GetHand(), CommunalCards));

	std::array<Card,5> BestHand = BettingHands[0];

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
	auto Itr = _Participants.begin();
	while (Itr != _Participants.end())
	{
		if ((*Itr)->GetOwner()->GetIsBroke() || (*Itr)->GetOwner()->GetIsFolded())
			Itr = _Participants.erase(Itr);
		else
			++Itr;
	}

	if (_Participants.size() == 1)
	{
		_Winners = _Participants;
		return;
	}

	std::vector<std::array<Card, 5>> BettingHands;

	for (auto const& Participant : _Participants)
		BettingHands.push_back(Evaluator->GetBestCommunalHand(Participant->GetOwner()->GetHand(), CommunalCards));

	std::array<Card, 5> BestHand = BettingHands[0];

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

void Table::SetMatch(const std::shared_ptr<Match>& _NewMatch)
{
	CurrentMatch = _NewMatch;
}

void Table::Print()
{
	std::cout << "\n--------------------------------------------------\n";
	
	std::cout << "Table: ";
	for(auto const& Card : CommunalCards)
		std::cout << " " << Card.To_String();
	
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
			std::cout << "P." << Index << " ($" << Players[Index]->GetStack() << "): " << Players[Index]->GetHandCardByIndex(0).To_String() << "," << Players[Index]->GetHand()[1].To_String() << "  |  $" << Players[Index]->GetAnte() << "\n";
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
