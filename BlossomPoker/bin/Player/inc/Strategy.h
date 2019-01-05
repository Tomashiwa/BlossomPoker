#pragma once
#include <iomanip>
#include <memory>

#include "../../Player/inc/Strategy.h"
#include "../../Table/inc/RaiseBetSize.h"
#include "../../AI/inc/Snapshot.h"

class Strategy
{
public:
	Strategy(Phase _Corresponding);
	~Strategy();

	BettingAction DetermineAction(std::vector<BettingAction> _AvaliableActions, float _WinRate, unsigned int _PlayerAmt);

	Phase GetPhase() {return CorrespondingPhase;}

	std::array<float, 4> GetThresholds() { return Thresholds; }
	void SetThreshold(unsigned int _Index, float _Threshold) 
	{ 
		Thresholds[_Index] = _Threshold; 
	}
	void SetThresholds(std::array<float, 4> _Thresholds) { Thresholds = _Thresholds; }

	RaiseBetSize GetSizing() { return Sizing; }

private:
	Phase CorrespondingPhase;

	std::array<float, 4> Thresholds;
	// 0 - Call
	// 1 - Half-Pot Raise
	// 2 - Pot Raise
	// 3 - All-In
	std::array<float, 4> Requirements;

	RaiseBetSize Sizing = RaiseBetSize::NONE;

	void CalculateRequirements(unsigned int _PlayerAmt);
	
};

