#include "../inc/Player.h"
#include "../../Cards/inc/Card.h"
#include "../../Table/inc/Table.h"

Player::Player(const std::shared_ptr<Table>& _Table, unsigned int _Index)
	: ResidingTable(_Table), Index(_Index), Stack(0), Ante(0), CurrentAction(BettingAction::NONE)
{}

void Player::Initialize()
{}

void Player::Update()
{
	CurrentAction = DetermineAction();
}

void Player::End()
{}

void Player::Reset()
{
	Ante = 0;
	Stack = 0;
	PotContribution = 0;

	CurrentAction = BettingAction::NONE;

	IsContributing = false;
	IsFolded = false;
	IsBroke = false;
}

void Player::SetHand(Card&  _First, Card& _Second)
{
	Hand[0] = _First;
	Hand[1] = _Second;
}

std::string Player::GetHandInfo()
{
	return Hand[0].To_String() + "," + Hand[1].To_String();
}

float Player::SetFitnessAsOverallRank(unsigned int _TourAmt)
{
	Fitness = ((float) Ranking / (float) (_TourAmt * 9));
	return Fitness;
}

float Player::CalculateFitness()
{
	//Profit per Hand = (MoneyWon - MoneyLost) / (HandsWon + HandsLost)
	Fitness = ((float)MoneyWon - (float)MoneyLost) / (float)(HandsWon + HandsLost);
	return Fitness;
}

void Player::ClearStats()
{
	Fitness = 0.0f;

	MoneyWon = 0;
	MoneyLost = 0;
	HandsWon = 0;
	HandsLost = 0;

	Profits = 0;
}

void Player::GetAvaliableActions(std::vector<BettingAction>& _PossibleActions)
{
	if (IsBroke || IsFolded || !IsContributing)
		return;

	_PossibleActions.push_back(BettingAction::Fold);

	if (Ante >= ResidingTable->GetRequiredAnte())
	{
		_PossibleActions.push_back(BettingAction::Check);

		if (Stack > 0)
			_PossibleActions.push_back(ResidingTable->GetRequiredAnte() > 0 ? BettingAction::Raise : BettingAction::Bet);
	}
	else
	{
		_PossibleActions.push_back(BettingAction::Call);

		if (Stack > (ResidingTable->GetRequiredAnte() - Ante))
			_PossibleActions.push_back(BettingAction::Raise);
	}
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

