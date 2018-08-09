#include "Strategy.h"
#include "BlossomAI.h"
#include "Orchastrator.h"

Strategy::Strategy(Orchastrator* _Orchastrator, double _CallingThresh, double _RaisingThresh)
{
	Orch = _Orchastrator;

	Thr_Calling = _CallingThresh;
	Thr_RaisingBetting = _RaisingThresh;
}

Strategy::~Strategy()
{
}

void Strategy::CalculateMWRs()
{
	Snapshot CurrentShot = Orch->GetAI()->GetSnapshot();
	//std::cout << "Current Pot: " << CurrentShot.Pot << " \n";

	double Mean = 100.0 / CurrentShot.PlayerAmt;
	double Diff = 100.0 - Mean;
	double Flactuation = Diff / 2.0;

	MWR_Calling = Mean + (Thr_Calling * Flactuation);
	MWR_RaisingBetting = Mean + (Thr_RaisingBetting * Flactuation);

	MWR_Calling = MWR_Calling > 100.0 ? 100.0 : MWR_Calling;
	MWR_RaisingBetting = MWR_RaisingBetting > 100.0 ? 100.0 : MWR_RaisingBetting;

	//std::cout << "Min Calling Rate: " << MinWinRate_Calling << " / Min Raising & Betting Rate: " << MinWinRate_RaisingBetting << std::endl;

	//if (CurrentShot.Pot == 0 || CurrentShot.RequiredAnte == 0)
	//{
	//	MinWinRate_Calling = Thr_Calling * (100.0 / CurrentShot.PlayerAmt);
	//	MinWinRate_RaisingBetting = Thr_RaisingBetting * (100.0 / CurrentShot.PlayerAmt);

	//	MinWinRate_Calling = MinWinRate_Calling > 100.0 ? 100.0 : MinWinRate_Calling;
	//	MinWinRate_RaisingBetting = MinWinRate_RaisingBetting > 100.0 ? 100.0 : MinWinRate_RaisingBetting;

	//	std::cout << "No bets were made yet... (Min Calling Rate: " << MinWinRate_Calling << " / Min Raising & Betting Rate: " << MinWinRate_RaisingBetting << ") \n";
	//	return;
	//}

	//double CallValue = (double)CurrentShot.RequiredAnte - (double)CurrentShot.CurrentAnte;
	//double PotOdds = CallValue / (CallValue + (double) CurrentShot.Pot);
	//MinWinRate_Calling = Thr_Calling * PotOdds * 100.0;
	//MinWinRate_RaisingBetting = Thr_RaisingBetting * PotOdds * 100.0;

	//MinWinRate_Calling = MinWinRate_Calling > 100.0 ? 100.0 : MinWinRate_Calling;
	//MinWinRate_RaisingBetting = MinWinRate_RaisingBetting > 100.0 ? 100.0 : MinWinRate_RaisingBetting;

	//std::cout << "Required Ante: " << CurrentShot.RequiredAnte << " / Current Ante: " << CurrentShot.CurrentAnte << " / Pot: " << CurrentShot.Pot << " / CallValue: " << CallValue << "\n";
	//std::cout << "Pot Odds: " << PotOdds << " / Min Calling Rate: " << MinWinRate_Calling << " / Min Raising & Betterng Rate: " << MinWinRate_RaisingBetting << " \n";
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

	double CurrentWinRate = Orch->GetAI()->DetermineWinRate();
	//std::cout << "Win Rate of " << Orch->GetAI()->GetSnapshot().Hole[0]->GetInfo() << "," << Orch->GetAI()->GetSnapshot().Hole[1]->GetInfo() << ": " << CurrentWinRate << "% | CallMin: " << MinWinRate_Calling << "% / RaiseMin: " << MinWinRate_RaisingBetting<< "%" << std::endl;

	if (CurrentWinRate >= MWR_RaisingBetting)
		return IsActionAvaliable(BettingAction::Bet) ? BettingAction::Bet : BettingAction::Raise;

	else if (IsActionAvaliable(BettingAction::Call) && CurrentWinRate >= MWR_Calling)
		return BettingAction::Call;

	else
		return IsActionAvaliable(BettingAction::Check) ? BettingAction::Check : BettingAction::Fold;

	return BettingAction::NONE;
}

std::array<double, 2> Strategy::GetThresholds()
{
	std::array<double, 2> Thresholds;
	Thresholds[0] = Thr_Calling;
	Thresholds[1] = Thr_RaisingBetting;
	return Thresholds;
}

void Strategy::PrintThresholds()
{
	std::cout << "Thr_Calling: " << Thr_Calling << " / Thr_RaisingBetting: " << Thr_RaisingBetting << "\n";
}