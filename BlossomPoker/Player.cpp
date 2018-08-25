#include "Player.h"
#include "Card.h"
#include "Board.h"

Player::Player(Board* _Board, unsigned int _Index)
{
	Index = _Index;
	Stack = 0;
	Ante = 0;

	SetBoard(_Board);
	Action = BettingAction::NONE;

	AI = new BlossomAI(_Board->GetEvaluator());
}

Player::Player(Board* _Board, unsigned int _Index, std::array<double, 8> _Thresholds)
{
	Index = _Index;
	Stack = 0;
	Ante = 0;

	SetBoard(_Board);
	Action = BettingAction::NONE;

	AI = new BlossomAI(_Board->GetEvaluator(), _Thresholds);
}

Player::~Player()
{
}

void Player::Start()
{

}

void Player::Update()
{
	Action = DetermineAction();
}

void Player::End()
{

}

void Player::Reset()
{
	Hand[0] = nullptr;
	Hand[1] = nullptr;

	Stack = 0;
	Ante = 0;
	PotContribution = 0;

	Action = BettingAction::NONE;

	IsParticipating = false;
	IsFolded = false;
	IsBroke = false;
}

std::vector<BettingAction> Player::GetAvaliableActions()
{
	std::vector<BettingAction> Actions;
	if (IsBroke || IsFolded || !IsParticipating) return Actions;

	Actions.push_back(BettingAction::Fold);

	if (SelfBoard->GetState() == Phase::Preflop)
	{
		Actions.push_back(BettingAction::Call);
		Actions.push_back(BettingAction::Raise);
	}
	else
	{
		if (SelfBoard->GetRequiredAnte() <= 0)
		{
			Actions.push_back(BettingAction::Check);
			Actions.push_back(BettingAction::Bet);
		}
		else
		{
			Actions.push_back(BettingAction::Call);
			Actions.push_back(BettingAction::Raise);
		}
	}

	return Actions;
}

BettingAction Player::DetermineAction()
{
	Snapshot NewShot;
	NewShot.Stack = GetStack();
	NewShot.RequiredAnte =  SelfBoard->GetRequiredAnte();
	NewShot.Pot = SelfBoard->GetPot();
	NewShot.PlayerAmt = 2;
	NewShot.BB = SelfBoard->GetBigBlind();
	NewShot.Phase = SelfBoard->GetState();
	NewShot.Hole = GetHand();
	NewShot.CurrentAnte = GetAnte();
	NewShot.Contribution = GetPotContribution();
	NewShot.Communal = SelfBoard->GetCommunalCards();
	NewShot.AvaliableActions = GetAvaliableActions();

	return AI->EnquireAction(NewShot);
}

void Player::SetBoard(Board* _Board)
{
	SelfBoard = _Board;
}

void Player::EmptyHand()
{
	Hand[0] = nullptr;
	Hand[1] = nullptr;
}

void Player::SetHand(Card* _First, Card* _Second)
{
	Hand[0] = _First;
	Hand[1] = _Second;
}

void Player::SetStack(unsigned int _Stack)
{
	Stack = _Stack;
}

void Player::SetAnte(unsigned int _Amt)
{
	if (_Amt == 0)
	{
		Ante = 0;
		return;
	}

	unsigned int AmtIncreased = _Amt - Ante;

	if (AmtIncreased < Stack)
	{
		Ante += AmtIncreased;
		PotContribution += AmtIncreased;

		Stack -= AmtIncreased;
	}
	else if (AmtIncreased >= Stack)
	{
		Ante += Stack;
		PotContribution += Stack;

		Stack = 0;
	}
}

void Player::EmptyPotContributon()
{
	PotContribution = 0;
}

void Player::SetAction(BettingAction _Action)
{
	Action = _Action;
}

void Player::SetIsParticipating(bool _IsParticipating)
{
	IsParticipating = _IsParticipating;
}

void Player::SetIsFolded(bool _IsFolded)
{
	IsFolded = _IsFolded;
}

void Player::SetIsBroke(bool _IsBroke)
{
	IsBroke = _IsBroke;
}

std::string Player::GetHandInfo()
{
	return Hand[0]->GetInfo() + "," + Hand[1]->GetInfo();
}