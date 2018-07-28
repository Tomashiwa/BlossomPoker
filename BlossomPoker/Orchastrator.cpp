#include "Orchastrator.h"
#include "BlossomAI.h"
#include "Strategy.h"

Orchastrator::Orchastrator(BlossomAI* _AI)
{
	AI = _AI;

	Strategies[0] = new Strategy(this, 0.3, 0.75);
	Strategies[1] = new Strategy(this, 0.3, 0.75);
	Strategies[2] = new Strategy(this, 0.3, 0.75);
	Strategies[3] = new Strategy(this, 0.3, 0.75);
	
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
