#pragma once
#include "Snapshot.h"
#include "BettingAction.h"
#include "Deck.h"

#include <vector>
#include <limits>

class Orchastrator;
class HandEvaluator;

class BlossomAI
{
public:
	BlossomAI(HandEvaluator* _Evaluator);
	~BlossomAI();

	BettingAction EnquireAction(Snapshot _Snapshot);
	std::vector<BettingAction> GetAvaliableActions();

	void UpdateSnapshot(Snapshot _New);
	Snapshot GetSnapshot() { return CurrentSnap; }

	double DetermineWinRate();

private:
	Orchastrator* Orch;
	HandEvaluator* Eval;
	Snapshot CurrentSnap;

	void PrintSnapshot(Snapshot _Shot);
};

