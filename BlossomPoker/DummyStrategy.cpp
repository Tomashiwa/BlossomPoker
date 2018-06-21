#include "DummyStrategy.h"
#include "DummyAI.h"
#include "DummyOrchastrator.h"

DummyStrategy::DummyStrategy(DummyOrchastrator* _Orchastrator, float _CallingThresh, float _RaisingThresh)
{
	Orchastrator = _Orchastrator;
	Thresh_Calling = _CallingThresh;
	Thresh_RaisingBetting = _RaisingThresh;

	CalculateMinWinRates();
}

DummyStrategy::~DummyStrategy()
{
}
	
void DummyStrategy::RenewSnapshot(Snapshot _NewShot)
{
	CurrentShot = _NewShot;
}

void DummyStrategy::CalculateMinWinRates()
{
	float MeanWinRate = 1.0 / CurrentShot.PlayerAmt;

	MinWinRate_Calling = Thresh_Calling * MeanWinRate;
	MinWinRate_RaisingBetting = Thresh_RaisingBetting * MeanWinRate;
}

bool DummyStrategy::IsActionAvaliable(BettingAction _Action)
{
	for (unsigned int Index = 0; Index < CurrentShot.AvaliableActions.size(); Index++)
	{
		if (CurrentShot.AvaliableActions[Index] == _Action)
			return true;
	}

	return false;
}

BettingAction DummyStrategy::DetermineIdealAction()
{
	float CurrentWinRate = Orchastrator->DetermineWinRate();

	if (CurrentWinRate >= MinWinRate_RaisingBetting)
		return IsActionAvaliable(BettingAction::Bet) ? BettingAction::Bet : BettingAction::Raise;

	else if (CurrentWinRate >= MinWinRate_Calling)
		return BettingAction::Call;

	else
		return IsActionAvaliable(BettingAction::Check) ? BettingAction::Check : BettingAction::Fold;

	return BettingAction::NONE;
}