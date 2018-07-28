#include "BlossomAI.h"
#include "HandEvaluator.h"
#include "Deck.h"
#include "Orchastrator.h"

BlossomAI::BlossomAI(HandEvaluator* _Evaluator)
{
	Orch = new Orchastrator(this);
	Eval = _Evaluator;
}


BlossomAI::~BlossomAI()
{
}

BettingAction BlossomAI::EnquireAction(Snapshot _Snapshot)
{
	CurrentSnap = _Snapshot;

	BettingAction ActionToTake = Orch->DetermineAction();
	return ActionToTake;
}

std::vector<BettingAction> BlossomAI::GetAvaliableActions()
{
	std::vector<BettingAction> Actions;

	Actions.push_back(BettingAction::Fold);

	if (CurrentSnap.Phase == Phase::Preflop)
	{
		if (CurrentSnap.RequiredAnte - CurrentSnap.CurrentAnte <= 0)
			Actions.push_back(BettingAction::Check);
		else
			Actions.push_back(BettingAction::Call);

		Actions.push_back(BettingAction::Raise);
	}
	else
	{
		if (CurrentSnap.RequiredAnte - CurrentSnap.CurrentAnte <= 0)
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

void BlossomAI::UpdateSnapshot(Snapshot _New)
{
	CurrentSnap = _New;
}

double BlossomAI::DetermineWinRate()
{
	return Eval->DetermineOdds_MonteCarlo(CurrentSnap.Hole, CurrentSnap.Communal,2500);
}

void BlossomAI::PrintSnapshot(Snapshot _Shot)
{
	std::cout << "Snapshot's Info: " << std::endl;
	std::cout << "Phase: ";
	if (_Shot.Phase == Phase::Preflop)
		std::cout << "Pre-flop" << std::endl;
	else if (_Shot.Phase == Phase::Flop)
		std::cout << "Flop" << std::endl;
	else if (_Shot.Phase == Phase::Turn)
		std::cout << "Turn" << std::endl;
	else if (_Shot.Phase == Phase::River)
		std::cout << "River" << std::endl;

	std::cout << "Communal Cards: ";
	for (unsigned int Index = 0; Index < _Shot.Communal.size(); Index++)
	{
		if (_Shot.Communal[Index] != nullptr)
			std::cout << _Shot.Communal[Index]->GetInfo() << " ";
	}
	std::cout << std::endl;

	std::cout << "Amt of Players: " << _Shot.PlayerAmt << std::endl;
	std::cout << "Pot:" << _Shot.Pot << std::endl;
	std::cout << "Required Ante: " << _Shot.RequiredAnte << std::endl;
	
	std::cout << "Hole Cards: " << _Shot.Hole[0]->GetInfo() << "," << _Shot.Hole[1]->GetInfo() << std::endl;
	std::cout << "Avaliable Actions: " << _Shot.AvaliableActions.size() << std::endl;
	std::cout << "Stack: " << _Shot.Stack << std::endl;
	std::cout << "Current Ante:" << _Shot.CurrentAnte << std::endl;
	std::cout << "Pot Contribution: " << _Shot.Contribution << std::endl;
}
