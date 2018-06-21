#pragma once
#include "BettingAction.h"
#include "Snapshot.h"

class DummyStrategy
{
public:
	DummyStrategy(DummyOrchastrator* _Orchastrator, float _CallingThresh, float _RaisingThresh);
	~DummyStrategy();

	void RenewSnapshot(Snapshot _NewShot);
	BettingAction DetermineIdealAction();
	
private:
	void CalculateMinWinRates();
	bool IsActionAvaliable(BettingAction _Action);

	DummyOrchastrator* Orchastrator;
	Snapshot CurrentShot;

	float Thresh_Calling;
	float Thresh_RaisingBetting;
	
	float MinWinRate_Calling;
	float MinWinRate_RaisingBetting;
};

