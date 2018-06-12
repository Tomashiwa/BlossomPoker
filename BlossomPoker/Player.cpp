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
	std::vector<BettingAction> AvaliableActions = GetAvaliableActions();
	if (AvaliableActions.size() == 0) return BettingAction::NONE;

	//PLACEHOLDER: ALWAYS CHECK OR CALL
	return AvaliableActions[1];
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