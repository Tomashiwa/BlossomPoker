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

	std::vector<BettingAction> GetAvaliableActions();
	BettingAction EnquireAction(Snapshot _Snapshot);

	Snapshot GetSnapshot() { return CurrentSnapshot; }

private:
	DummyOrchastrator* Orchastrator;
	Snapshot CurrentSnapshot;
};

