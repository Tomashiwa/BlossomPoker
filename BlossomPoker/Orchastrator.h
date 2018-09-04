#pragma once
#include <chrono>
#include <random>
#include <time.h>
#include <memory>

#include "BettingAction.h"
#include "Snapshot.h"

class BlossomAI;
class Strategy;

class Orchastrator : public std::enable_shared_from_this<Orchastrator>
{
public:
	Orchastrator(std::shared_ptr<BlossomAI> _AI);

	~Orchastrator();

	void InitializeStrat(std::array<double,8> _Thresholds);
	void InitializeRandomStrat();

	void SetOptimalStrategy(std::shared_ptr<Strategy>& _Strategy);
	BettingAction DetermineAction();
	
	std::array<double, 8> GetThresholds();
	std::shared_ptr<BlossomAI> GetAI() { return AI; }

private:
	std::shared_ptr<BlossomAI> AI;
	std::shared_ptr<Strategy> CurrentStrategy;

	std::array<std::shared_ptr<Strategy>, 4> Strategies;
};

