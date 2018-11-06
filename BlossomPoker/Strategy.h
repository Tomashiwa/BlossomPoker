#pragma once
#include <iomanip>
#include <memory>

#include "BettingAction.h"
#include "Snapshot.h"

class Orchastrator;

class Strategy
{
public:
	Strategy(std::shared_ptr<Orchastrator> _Orchastrator, float _CalingThresh, float _RaisingThresh);
	~Strategy();

	BettingAction DetermineIdealAction();

	void PrintThresholds();
	void SetThreshold(unsigned int _Index, float _Value) { Thresholds[_Index] = _Value; }
	std::array<float, 2> GetThresholds() { return Thresholds; }

private:
	std::shared_ptr<Orchastrator> Orch;

	std::array<float, 2> Thresholds; // 0 => Calling, 1 => Raising and Betting
	std::array<float, 2> MinWinRates; // 0 => Calling, 1 =? Raising and Betting

	void CalculateMWRs();
	bool IsActionAvaliable(BettingAction _Action);
};

