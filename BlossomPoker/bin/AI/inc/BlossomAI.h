#pragma once
#include <vector>
#include <array>
#include <memory>
#include <random>
#include <chrono>

#include "../../Cards/inc/HandEvaluator.h"
#include "../../AI/inc/BlossomAI.h"
#include "../../Player/inc/Strategy.h"
#include "../../Table/inc/BettingAction.h"

class BlossomAI
{
public:
	BlossomAI();
	~BlossomAI();

	void Initialise();
	void Reset();

	BettingAction EnquireAction(Snapshot _Snapshot);
	float DetermineWinRate(std::array<Card,2> _Hole, std::vector<Card> _Communal, unsigned int _OppoAmt);

	void SetThresholds(std::array<float, 16> _Thresholds);
	void SetThresholdByPhase(Phase _Phase, unsigned int _Index, float _Threshold);
	void SetThresholdsByPhase(Phase _Phase, std::array<float, 4> _Thresholds);
	std::array<float, 16> GetThresholds() { return Thresholds; }
	std::array<float, 4> GetThresholdsByPhase(Phase _Phase);

	std::shared_ptr<Strategy> GetStrategy(Phase _Phase);
	std::shared_ptr<Strategy> GetActiveStrategy() { return ActiveStrategy; }

	void SetEvalutor(const std::shared_ptr<HandEvaluator>& _Evaluator) { Evaluator = _Evaluator; }

	unsigned int GetRaiseBetAmt() { return RaiseBetAmt; }

private:
	std::shared_ptr<Strategy> ActiveStrategy;
	std::array<float, 16> Thresholds;
	std::array<std::shared_ptr<Strategy>, 4> Strategies;

	unsigned int RaiseBetAmt = 0;

	std::mt19937 MT;
	std::shared_ptr<HandEvaluator> Evaluator;
};

