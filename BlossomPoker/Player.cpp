#include "Player.h"
#include "Card.h"
#include "Board.h"

Player::Player(std::shared_ptr<Board> _Board, unsigned int _Index)
{
	Index = _Index;
	Stack = 0;
	Ante = 0;

	SetBoard(_Board);
	Action = BettingAction::NONE;

	AI = std::make_shared<BlossomAI>();
	AI->Initialize(_Board->GetEvaluator());
}

Player::Player(std::shared_ptr<Board> _Board, unsigned int _Index, std::array<double, 8> _Thresholds)
{
	Index = _Index;
	Stack = 0;
	Ante = 0;

	SetBoard(_Board);
	Action = BettingAction::NONE;

	AI = std::make_shared<BlossomAI>();
	AI->InitializeWithThreshold(_Board->GetEvaluator(),_Thresholds);
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
	IsParticipating = false;
}

std::shared_ptr<Card> Player::GetHandCardByIndex(unsigned int _Index)
{
	return Hand[_Index];
}

void Player::GetAvaliableActions(std::vector<BettingAction>& _PossibleActions)
{
	if (IsBroke || IsFolded || !IsParticipating) 
		return;

	_PossibleActions.push_back(BettingAction::Fold);

	if (SelfBoard->GetState() == Phase::Preflop)
	{
		_PossibleActions.push_back(BettingAction::Call);
		_PossibleActions.push_back(BettingAction::Raise);
	}
	else
	{
		if (SelfBoard->GetRequiredAnte() <= 0)
		{
			_PossibleActions.push_back(BettingAction::Check);
			_PossibleActions.push_back(BettingAction::Bet);
		}
		else
		{
			_PossibleActions.push_back(BettingAction::Call);
			_PossibleActions.push_back(BettingAction::Raise);
		}
	}
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
	
	GetAvaliableActions(NewShot.AvaliableActions);

	return AI->EnquireAction(NewShot);
}

void Player::SetBoard(std::shared_ptr<Board> _Board)
{
	SelfBoard = _Board;
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