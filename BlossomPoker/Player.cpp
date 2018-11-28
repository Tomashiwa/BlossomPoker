#include "Player.h"
#include "Card.h"
#include "Table.h"

Player::Player(const std::shared_ptr<Table>& _Table, unsigned int _Index)
	: ResidingTable(_Table), Index(_Index), Stack(0), Ante(0), CurrentAction(BettingAction::NONE)
{
	AI = std::make_shared<BlossomAI>();
	AI->Initialise();
}

Player::~Player()
{
}

void Player::Start()
{
}

void Player::Update()
{
	CurrentAction = DetermineAction();
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

	CurrentAction = BettingAction::NONE;

	IsContributing = false;
	IsFolded = false;
	IsBroke = false;
}

void Player::SetHand(const std::shared_ptr<Card>&  _First, const std::shared_ptr<Card>& _Second)
{
	Hand[0] = std::move(_First);
	Hand[1] = std::move(_Second);
}

std::string Player::GetHandInfo()
{
	return Hand[0]->GetInfo() + "," + Hand[1]->GetInfo();
}

void Player::EmptyHand()
{
	Hand[0].reset();
	Hand[1].reset();
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

BettingAction Player::DetermineAction()
{
	if (AI == nullptr)
		return BettingAction::Fold;

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

	//std::cout << "Enquiring BlossomAI of P." << Index << "...\n";
	return AI->EnquireAction(NewShot);
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

