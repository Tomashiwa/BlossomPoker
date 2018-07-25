#include "DummyStrategy.h"
#include "DummyAI.h"
#include "DummyOrchastrator.h"

DummyStrategy::DummyStrategy(DummyOrchastrator* _Orchastrator, double _CallingThresh, double _RaisingThresh)
{
	Orchastrator = _Orchastrator;

	Thresh_Calling = _CallingThresh;
	Thresh_RaisingBetting = _RaisingThresh;
}

DummyStrategy::~DummyStrategy()
{
}

void DummyStrategy::CalculateMinWinRates()
{
	Snapshot CurrentShot = Orchastrator->GetAI()->GetSnapshot();
	//std::cout << "Current Pot: " << CurrentShot.Pot << " \n";

	double Mean = 100.0 / CurrentShot.PlayerAmt;
	double Diff = 100.0 - Mean;
	double Flactuation = Diff / 2.0;

	MinWinRate_Calling = Mean + (Thresh_Calling * Flactuation);
	MinWinRate_RaisingBetting = Mean + (Thresh_RaisingBetting * Flactuation);

	MinWinRate_Calling = MinWinRate_Calling > 100.0 ? 100.0 : MinWinRate_Calling;
	MinWinRate_RaisingBetting = MinWinRate_RaisingBetting > 100.0 ? 100.0 : MinWinRate_RaisingBetting;

	//std::cout << "Min Calling Rate: " << MinWinRate_Calling << " / Min Raising & Betting Rate: " << MinWinRate_RaisingBetting << std::endl;

	//if (CurrentShot.Pot == 0 || CurrentShot.RequiredAnte == 0)
	//{
	//	MinWinRate_Calling = Thresh_Calling * (100.0 / CurrentShot.PlayerAmt);
	//	MinWinRate_RaisingBetting = Thresh_RaisingBetting * (100.0 / CurrentShot.PlayerAmt);

	//	MinWinRate_Calling = MinWinRate_Calling > 100.0 ? 100.0 : MinWinRate_Calling;
	//	MinWinRate_RaisingBetting = MinWinRate_RaisingBetting > 100.0 ? 100.0 : MinWinRate_RaisingBetting;

	//	std::cout << "No bets were made yet... (Min Calling Rate: " << MinWinRate_Calling << " / Min Raising & Betting Rate: " << MinWinRate_RaisingBetting << ") \n";
	//	return;
	//}

	//double CallValue = (double)CurrentShot.RequiredAnte - (double)CurrentShot.CurrentAnte;
	//double PotOdds = CallValue / (CallValue + (double) CurrentShot.Pot);
	//MinWinRate_Calling = Thresh_Calling * PotOdds * 100.0;
	//MinWinRate_RaisingBetting = Thresh_RaisingBetting * PotOdds * 100.0;

	//MinWinRate_Calling = MinWinRate_Calling > 100.0 ? 100.0 : MinWinRate_Calling;
	//MinWinRate_RaisingBetting = MinWinRate_RaisingBetting > 100.0 ? 100.0 : MinWinRate_RaisingBetting;

	//std::cout << "Required Ante: " << CurrentShot.RequiredAnte << " / Current Ante: " << CurrentShot.CurrentAnte << " / Pot: " << CurrentShot.Pot << " / CallValue: " << CallValue << "\n";
	//std::cout << "Pot Odds: " << PotOdds << " / Min Calling Rate: " << MinWinRate_Calling << " / Min Raising & Betterng Rate: " << MinWinRate_RaisingBetting << " \n";
}

bool DummyStrategy::IsActionAvaliable(BettingAction _Action)
{
	Snapshot CurrentShot = Orchastrator->GetAI()->GetSnapshot();

	for (unsigned int Index = 0; Index < CurrentShot.AvaliableActions.size(); Index++)
	{
		if (CurrentShot.AvaliableActions[Index] == _Action)
			return true;
	}

	return false;
}

BettingAction DummyStrategy::DetermineIdealAction()
{
	CalculateMinWinRates();

	double CurrentWinRate = Orchastrator->GetAI()->DetermineWinRate();
	std::cout << "Win Rate of " << Orchastrator->GetAI()->GetSnapshot().Hole[0]->GetInfo() << "," << Orchastrator->GetAI()->GetSnapshot().Hole[1]->GetInfo() << ": " << CurrentWinRate << "% | CallMin: " << MinWinRate_Calling << "% / RaiseMin: " << MinWinRate_RaisingBetting<< "%" << std::endl;

	if (CurrentWinRate >= MinWinRate_RaisingBetting)
		return IsActionAvaliable(BettingAction::Bet) ? BettingAction::Bet : BettingAction::Raise;

	else if (IsActionAvaliable(BettingAction::Call) && CurrentWinRate >= MinWinRate_Calling)
		return BettingAction::Call;

	else
		return IsActionAvaliable(BettingAction::Check) ? BettingAction::Check : BettingAction::Fold;

	return BettingAction::NONE;
}