#pragma once
#include <iomanip>
#include <memory>

#include "BettingAction.h"
#include "Snapshot.h"

class Orchastrator;

class Strategy
{
public:
	Strategy(std::shared_ptr<Orchastrator> _Orchastrator, double _CalingThresh, double _RaisingThresh);
	~Strategy();

	BettingAction DetermineIdealAction();

	void PrintThresholds();
	void SetThreshold(unsigned int _Index, double _Value) { Thresholds[_Index] = _Value; }
	std::array<double, 2> GetThresholds() { return Thresholds; }

private:
	std::shared_ptr<Orchastrator> Orch;

	std::array<double, 2> Thresholds; // 0 => Calling, 1 => Raising and Betting
	std::array<double, 2> MinWinRates; // 0 => Calling, 1 =? Raising and Betting

	void CalculateMWRs();
	bool IsActionAvaliable(BettingAction _Action);
};

