#pragma once
#include <chrono>
#include <random>
#include <time.h>

#include "BettingAction.h"
#include "Snapshot.h"

class BlossomAI;
class Strategy;

class Orchastrator
{
public:
	Orchastrator(BlossomAI* _AI);
	Orchastrator(BlossomAI* _AI, std::array<double,8> _Thresholds);

	~Orchastrator();

	Strategy* DetermineOptimalStrategy();
	BettingAction DetermineAction();
	
	std::array<double, 8> GetThresholds();
	BlossomAI* GetAI() { return AI; }

private:
	BlossomAI* AI;

	Strategy* CurrentStrategy;
	std::array<Strategy*, 4> Strategies;
};

