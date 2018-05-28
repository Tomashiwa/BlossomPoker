#include "Board.h"
#include "Player.h"

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
	std::vector<BettingAction> AvaliableActions;
	if (IsBroke || IsFolded || !IsBetting) return AvaliableActions;

	AvaliableActions.push_back(BettingAction::Fold);
	AvaliableActions.push_back(BettingAction::Raise);

	if (Ante < SelfBoard->GetRequiredAnte()) AvaliableActions.push_back(BettingAction::Call);
	else AvaliableActions.push_back(BettingAction::Check);

	return AvaliableActions;
}

BettingAction Player::DetermineAction()
{
	std::vector<BettingAction> AvaliableActions = GetAvaliableActions();
	if (AvaliableActions.size() == 0) return BettingAction::NONE;

	//PLACEHOLDER: ALWAYS CHECK OR CALL
	return AvaliableActions[2];
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

void Player::SetIsBetting(bool _IsBetting)
{
	IsBetting = _IsBetting;
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