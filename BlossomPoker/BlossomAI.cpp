#include "BlossomAI.h"
#include "HandEvaluator.h"
#include "Deck.h"
#include "Orchastrator.h"

BlossomAI::BlossomAI(HandEvaluator* _Evaluator)
{
	Orch = new Orchastrator(this);
	Eval = _Evaluator;
}

BlossomAI::BlossomAI(HandEvaluator* _Evaluator, std::array<double,8> _Thresholds)
{
	Orch = new Orchastrator(this, _Thresholds);
	Eval = _Evaluator;
}

BlossomAI::~BlossomAI()
{
}

BettingAction BlossomAI::EnquireAction(Snapshot _Snapshot)
{
	CurrentShot = _Snapshot;

	BettingAction ActionToTake = Orch->DetermineAction();
	return ActionToTake;
}

std::vector<BettingAction> BlossomAI::GetAvaliableActions()
{
	std::vector<BettingAction> Actions;

	Actions.push_back(BettingAction::Fold);

	if (CurrentShot.Phase == Phase::Preflop)
	{
		if (CurrentShot.RequiredAnte - CurrentShot.CurrentAnte <= 0)
			Actions.push_back(BettingAction::Check);
		else
			Actions.push_back(BettingAction::Call);
		
		if((CurrentShot.RequiredAnte - CurrentShot.CurrentAnte) + CurrentShot.BB <= CurrentShot.Stack)
			Actions.push_back(BettingAction::Raise);
	}
	else
	{
		if (CurrentShot.RequiredAnte - CurrentShot.CurrentAnte <= 0)
		{
			Actions.push_back(BettingAction::Check);

			if((CurrentShot.Phase == Phase::Flop && CurrentShot.Stack >= CurrentShot.BB) || ((CurrentShot.Phase == Phase::River || CurrentShot.Phase == Phase::Turn) && CurrentShot.Stack >= 2 * CurrentShot.BB))
				Actions.push_back(BettingAction::Bet);
		}
		else
		{
			Actions.push_back(BettingAction::Call);

			if ((CurrentShot.Phase == Phase::Flop && CurrentShot.Stack >= CurrentShot.BB) || ((CurrentShot.Phase == Phase::River || CurrentShot.Phase == Phase::Turn) && CurrentShot.Stack >= 2 * CurrentShot.BB))
				Actions.push_back(BettingAction::Raise);
		}
	}

	return Actions;
}

double BlossomAI::DetermineWinRate()
{
	return Eval->DetermineOdds_MonteCarlo(CurrentShot.Hole, CurrentShot.Communal,2500);
}

void BlossomAI::PrintShot()
{
	std::cout << "Snapshot's Info: \n";
	std::cout << "Phase: ";
	if (CurrentShot.Phase == Phase::Preflop)
		std::cout << "Pre-flop \n";
	else if (CurrentShot.Phase == Phase::Flop)
		std::cout << "Flop \n";
	else if (CurrentShot.Phase == Phase::Turn)
		std::cout << "Turn \n";
	else if (CurrentShot.Phase == Phase::River)
		std::cout << "River \n";

	std::cout << "Communal Cards: ";
	for (unsigned int Index = 0; Index < CurrentShot.Communal.size(); Index++)
	{
		if (CurrentShot.Communal[Index] != nullptr)
			std::cout << CurrentShot.Communal[Index]->GetInfo() << " ";
	}
	std::cout << "\n";

	std::cout << "Amt of Players: " << CurrentShot.PlayerAmt << "\n";
	std::cout << "Pot:" << CurrentShot.Pot << "\n";
	std::cout << "Required Ante: " << CurrentShot.RequiredAnte << "\n";
	
	std::cout << "Hole Cards: " << CurrentShot.Hole[0]->GetInfo() << "," << CurrentShot.Hole[1]->GetInfo() << "\n";
	std::cout << "Avaliable Actions: " << CurrentShot.AvaliableActions.size() << "\n";
	std::cout << "Stack: " << CurrentShot.Stack << "\n";
	std::cout << "Current Ante:" << CurrentShot.CurrentAnte << "\n";
	std::cout << "Pot Contribution: " << CurrentShot.Contribution << "\n";
}
