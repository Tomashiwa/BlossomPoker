#pragma once
#include "Snapshot.h"
#include "BettingAction.h"
#include "Deck.h"

#include <vector>
#include <limits>
#include <memory>

class Orchastrator;
class HandEvaluator;

class BlossomAI : public std::enable_shared_from_this<BlossomAI>
{
public:
	BlossomAI();
	~BlossomAI();

	void Initialize(std::shared_ptr<HandEvaluator> _Evaluator);
	void InitializeWithThreshold(std::shared_ptr<HandEvaluator> _Evaluator, std::array<float, 8> _Thresholds);

	//std::vector<BettingAction> GetAvaliableActions();
	BettingAction EnquireAction(const Snapshot& _Snapshot);

	void PrintShot();
	float DetermineEarningPotential();

	void SetThreshold(unsigned int _Index, float _Value);
	std::array<float, 8> GetThresholds();
	Snapshot GetSnapshot() { return CurrentShot; }

private:
	std::shared_ptr<Orchastrator> Orch;
	std::shared_ptr<HandEvaluator> Eval;

	Snapshot CurrentShot;

};

