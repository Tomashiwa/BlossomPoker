#include "DummyAI.h"
#include "DummyOrchastrator.h"
#include "HandEvaluator.h"
#include "Deck.h"

DummyAI::DummyAI(HandEvaluator* _Evaluator)
{
	Orchastrator = new DummyOrchastrator(this);
	Evaluator = _Evaluator;
}


DummyAI::~DummyAI()
{
}

BettingAction DummyAI::EnquireAction(Snapshot _Snapshot)
{
	CurrentSnapshot = _Snapshot;

	BettingAction ActionToTake = Orchastrator->DetermineAction();
	return ActionToTake;
}

std::vector<BettingAction> DummyAI::GetAvaliableActions()
{
	std::vector<BettingAction> Actions;

	Actions.push_back(BettingAction::Fold);

	if (CurrentSnapshot.Phase == Phase::Preflop)
	{
		if (CurrentSnapshot.RequiredAnte - CurrentSnapshot.CurrentAnte <= 0)
			Actions.push_back(BettingAction::Check);
		else
			Actions.push_back(BettingAction::Call);

		Actions.push_back(BettingAction::Raise);
	}
	else
	{
		if (CurrentSnapshot.RequiredAnte - CurrentSnapshot.CurrentAnte <= 0)
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

void DummyAI::UpdateSnapshot(Snapshot _New)
{
	CurrentSnapshot = _New;
}

double DummyAI::DetermineWinRate()
{
	return Evaluator->DetermineOdds_MonteCarlo(CurrentSnapshot.Hole, CurrentSnapshot.Communal,2500);
}

void DummyAI::PrintSnapshot(Snapshot _Shot)
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
