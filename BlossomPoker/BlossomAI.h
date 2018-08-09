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
	BlossomAI(HandEvaluator* _Evaluator, std::array<double, 8> _Thresholds);
	~BlossomAI();

	std::vector<BettingAction> GetAvaliableActions();
	BettingAction EnquireAction(Snapshot _Snapshot);

	void PrintShot();
	double DetermineWinRate();

	Snapshot GetSnapshot() { return CurrentShot; }
	std::array<double, 8> GetThresholds();

private:
	Orchastrator* Orch;
	HandEvaluator* Eval;
	Snapshot CurrentShot;

};

