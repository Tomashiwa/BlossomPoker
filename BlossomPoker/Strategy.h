#pragma once
#include <iomanip>

#include "BettingAction.h"
#include "Snapshot.h"

class Orchastrator;

class Strategy
{
public:
	Strategy(Orchastrator* _Orchastrator, double _CalingThresh, double _RaisingThresh);
	~Strategy();

	BettingAction DetermineIdealAction();
	void PrintThresholds();
	std::array<double, 2> GetThresholds();

private:
	Orchastrator* Orch;

	double Thr_Calling;
	double Thr_RaisingBetting;

	//MWR - Minimum Winning Rate required for a certain move
	double MWR_Calling;
	double MWR_RaisingBetting;

	void CalculateMWRs();
	bool IsActionAvaliable(BettingAction _Action);
};

