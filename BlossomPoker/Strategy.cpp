#include "Strategy.h"
#include "BlossomAI.h"
#include "Orchastrator.h"

Strategy::Strategy(std::shared_ptr<Orchastrator> _Orchastrator, float _CallingThresh, float _RaisingThresh)
	: Orch(_Orchastrator)
{
	Thresholds[0] = _CallingThresh;
	Thresholds[1] = _RaisingThresh;
}

Strategy::~Strategy()
{
}

void Strategy::CalculateMWRs()
{
	Snapshot CurrentShot = Orch->GetAI()->GetSnapshot();

	float Mean = 100.0 / CurrentShot.PlayerAmt;
	float Diff = 100.0 - Mean;
	float Flactuation = Diff / 2.0;

	MinWinRates[0] = Mean + (Thresholds[0] * Flactuation); // Calling
	MinWinRates[1] = Mean + (Thresholds[1] * Flactuation); // Raising and Betting

	MinWinRates[0] = MinWinRates[0] > 100.0 ? 100.0 : MinWinRates[0];
	MinWinRates[1] = MinWinRates[1] > 100.0 ? 100.0 : MinWinRates[1];
}

bool Strategy::IsActionAvaliable(BettingAction _Action)
{
	Snapshot CurrentShot = Orch->GetAI()->GetSnapshot();

	for (unsigned int Index = 0; Index < CurrentShot.AvaliableActions.size(); Index++)
	{
		if (CurrentShot.AvaliableActions[Index] == _Action)
			return true;
	}

	return false;
}

BettingAction Strategy::DetermineIdealAction()
{
	CalculateMWRs();

	float CurrentWinRate = Orch->GetAI()->DetermineEarningPotential();
	
	if (CurrentWinRate >= MinWinRates[1]) // Raising and Betting
		return IsActionAvaliable(BettingAction::Bet) ? BettingAction::Bet : BettingAction::Raise;

	else if (IsActionAvaliable(BettingAction::Call) && CurrentWinRate >= MinWinRates[0]) // Calling
		return BettingAction::Call;

	else
		return IsActionAvaliable(BettingAction::Check) ? BettingAction::Check : BettingAction::Fold;

	return BettingAction::NONE;
}

void Strategy::PrintThresholds()
{
	std::cout << "Thr_Calling: " << Thresholds[0] << " / Thr_RaisingBetting: " << Thresholds[1] << "\n";
}