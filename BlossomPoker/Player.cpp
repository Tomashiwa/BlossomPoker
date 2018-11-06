#include "Player.h"
#include "Card.h"
#include "Table.h"

Player::Player(std::shared_ptr<Table> _Table, unsigned int _Index)
	: ResidingTable(_Table), Index(_Index), Stack(0), Ante(0), Action(BettingAction::NONE)
{
	AI = std::make_shared<BlossomAI>();
	AI->Initialize(_Table->GetEvaluator());
}

Player::Player(std::shared_ptr<Table> _Table, unsigned int _Index, std::array<float, 8> _Thresholds)
{
	Index = _Index;
	Stack = 0;
	Ante = 0;

	SetTable(_Table);
	Action = BettingAction::NONE;

	AI = std::make_shared<BlossomAI>();
	AI->InitializeWithThreshold(_Table->GetEvaluator(),_Thresholds);
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
	EmptyHand();

	Ante = 0;
	Stack = 0;
	PotContribution = 0;

	Action = BettingAction::NONE;

	IsBroke = false;
	IsFolded = false;
	IsContributing = false;
}

void Player::GetAvaliableActions(std::vector<BettingAction>& _PossibleActions)
{
	if (IsBroke || IsFolded || !IsContributing)
		return;

	_PossibleActions.push_back(BettingAction::Fold);

	if (Ante < ResidingTable->GetRequiredAnte())
	{
		_PossibleActions.push_back(BettingAction::Call);

		if(Stack > 0)
			_PossibleActions.push_back(BettingAction::Raise);
	}
	else
	{
		_PossibleActions.push_back(BettingAction::Check);
		
		if(Stack > 0)
			_PossibleActions.push_back(BettingAction::Bet);
	}
}

BettingAction Player::DetermineAction()
{
	Snapshot NewShot;
	NewShot.Stack = GetStack();
	NewShot.RequiredAnte =  ResidingTable->GetRequiredAnte();
	NewShot.Pot = ResidingTable->GetPot();
	NewShot.PlayerAmt = ResidingTable->GetPlayers().size();
	NewShot.BB = ResidingTable->GetBigBlind();
	NewShot.Phase = ResidingTable->GetState();
	NewShot.Hole = GetHand();
	NewShot.CurrentAnte = GetAnte();
	NewShot.Contribution = GetPotContribution();
	NewShot.Communal = ResidingTable->GetCommunalCards();
	
	GetAvaliableActions(NewShot.AvaliableActions);

	return AI->EnquireAction(NewShot);
}

void Player::EmptyHand()
{
	Hand[0].reset();
	Hand[1].reset();
}

void Player::SetHand(std::shared_ptr<Card>  _First, std::shared_ptr<Card> _Second)
{
	Hand[0] = std::move(_First);
	Hand[1] = std::move(_Second);
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

std::string Player::GetHandInfo()
{
	return Hand[0]->GetInfo() + "," + Hand[1]->GetInfo();
}