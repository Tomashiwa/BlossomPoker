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

	std::vector<BettingAction> GetAvaliableActions();
	BettingAction EnquireAction(Snapshot _Snapshot);

	void PrintShot();
	double DetermineWinRate();

	Snapshot GetSnapshot() { return CurrentShot; }

private:
	Orchastrator* Orch;
	HandEvaluator* Eval;
	Snapshot CurrentShot;

};

