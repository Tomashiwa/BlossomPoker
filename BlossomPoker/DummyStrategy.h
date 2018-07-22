#pragma once
#include "BettingAction.h"
#include "Snapshot.h"

class DummyOrchastrator;

class DummyStrategy
{
public:
	DummyStrategy(DummyOrchastrator* _Orchastrator, double _CallingThresh, double _RaisingThresh);
	~DummyStrategy();

	BettingAction DetermineIdealAction();

private:
	DummyOrchastrator* Orchastrator;

	double Thresh_Calling;
	double Thresh_RaisingBetting;

	double MinWinRate_Calling;
	double MinWinRate_RaisingBetting;

	void CalculateMinWinRates();
	bool IsActionAvaliable(BettingAction _Action);
};

