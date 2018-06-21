#pragma once
#include "Snapshot.h"
#include "BettingAction.h"
#include <vector>

class DummyOrchastrator;

class DummyAI
{
public:
	DummyAI();
	~DummyAI();

	BettingAction EnquireAction(Snapshot _Snapshot);
	std::vector<BettingAction> GetAvaliableActions();

	void UpdateSnapshot(Snapshot _New);
	Snapshot GetSnapshot() { return CurrentSnapshot; }

	float DetermineWinRate();

private:
	DummyOrchastrator* Orchastrator;
	Snapshot CurrentSnapshot;
};

