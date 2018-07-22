#pragma once
#include "Snapshot.h"
#include "BettingAction.h"
#include "Deck.h"

#include <vector>
#include <limits>

class DummyOrchastrator;
class HandEvaluator;

class DummyAI
{
public:
	DummyAI(HandEvaluator* _Evaluator);
	~DummyAI();

	BettingAction EnquireAction(Snapshot _Snapshot);
	std::vector<BettingAction> GetAvaliableActions();

	void UpdateSnapshot(Snapshot _New);
	Snapshot GetSnapshot() { return CurrentSnapshot; }

	double DetermineWinRate();

private:
	DummyOrchastrator* Orchastrator;
	HandEvaluator* Evaluator;
	Snapshot CurrentSnapshot;

	void PrintSnapshot(Snapshot _Shot);
};

