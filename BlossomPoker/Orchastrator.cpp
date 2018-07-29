#include "Orchastrator.h"
#include "BlossomAI.h"
#include "Strategy.h"

Orchastrator::Orchastrator(BlossomAI* _AI)
{
	AI = _AI;

	//Use Mersenne Twister to obtain a random card to insert into Set
	auto Seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	std::mt19937 mt(Seed);
	std::uniform_real_distribution<double> ThresholdDistribution(0.0, 1.0);

	Strategies[0] = new Strategy(this, ThresholdDistribution(mt), ThresholdDistribution(mt));
	Strategies[1] = new Strategy(this, ThresholdDistribution(mt), ThresholdDistribution(mt));
	Strategies[2] = new Strategy(this, ThresholdDistribution(mt), ThresholdDistribution(mt));
	Strategies[3] = new Strategy(this, ThresholdDistribution(mt), ThresholdDistribution(mt));

	//std::cout << "Pre-flop: ";
	//Strategies[0]->PrintThresholds();
	//std::cout << "Flop: ";
	//Strategies[1]->PrintThresholds();
	//std::cout << "Turn: ";
	//Strategies[2]->PrintThresholds();
	//std::cout << "River: ";
	//Strategies[3]->PrintThresholds();
	
	CurrentStrategy = nullptr;
}

Orchastrator::Orchastrator(BlossomAI* _AI, std::array<double,8> _Thresholds)
{
	AI = _AI;

	Strategies[0] = new Strategy(this,_Thresholds[0], _Thresholds[1]);
	Strategies[1] = new Strategy(this,_Thresholds[2], _Thresholds[3]);
	Strategies[2] = new Strategy(this,_Thresholds[4], _Thresholds[5]);
	Strategies[3] = new Strategy(this,_Thresholds[6], _Thresholds[7]);

	CurrentStrategy = nullptr;
}

Orchastrator::~Orchastrator()
{
}

Strategy* Orchastrator::DetermineOptimalStrategy()
{
	switch (AI->GetSnapshot().Phase)
	{
		case Phase::Preflop:
			return Strategies[0];

		case Phase::Flop:
			return Strategies[1];

		case Phase::River:
			return Strategies[2];

		case Phase::Turn:
			return Strategies[3];
	}

	return nullptr;
}

BettingAction Orchastrator::DetermineAction()
{
	CurrentStrategy = DetermineOptimalStrategy();
	return CurrentStrategy->DetermineIdealAction();
}

