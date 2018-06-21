#include "DummyOrchastrator.h"
#include "DummyAI.h"
#include "DummyStrategy.h"

DummyOrchastrator::DummyOrchastrator(DummyAI* _AI)
{
	AI = _AI;

	Strategies[0] = new DummyStrategy(this,0.5,0.5);
	Strategies[1] = new DummyStrategy(this);
	Strategies[2] = new DummyStrategy(this);
	Strategies[3] = new DummyStrategy(this);

	CurrentStrategy = nullptr;
}

DummyOrchastrator::~DummyOrchastrator()
{
}

DummyStrategy* DummyOrchastrator::DetermineOptimalStrategy()
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

BettingAction DummyOrchastrator::DetermineAction()
{
	CurrentStrategy = DetermineOptimalStrategy();
	return CurrentStrategy->Evaluate();
}
