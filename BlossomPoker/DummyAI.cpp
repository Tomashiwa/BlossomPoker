#include "DummyAI.h"
#include "DummyOrchastrator.h"

DummyAI::DummyAI()
{
	Orchastrator = new DummyOrchastrator(this);
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
		Actions.push_back(BettingAction::Call);
		Actions.push_back(BettingAction::Raise);
	}
	else
	{
		if (CurrentSnapshot.RequiredAnte <= 0)
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

float DummyAI::DetermineWinRate()
{

}
