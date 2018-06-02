#include "DummyOrchastrator.h"
#include "DummyAI.h"
#include "DummyStrategy.h"

DummyOrchastrator::DummyOrchastrator(DummyAI* _AI)
{
	AI = _AI;

	Strategies[0] = new DummyStrategy();
	Strategies[1] = new DummyStrategy();
	Strategies[2] = new DummyStrategy();
	Strategies[3] = new DummyStrategy();

	CurrentStrategy = nullptr;
}

DummyOrchastrator::~DummyOrchastrator()
{
}

DummyStrategy* DummyOrchastrator::DetermineOptimalStrategy()
{
	switch (AI->GetSnapshot().Phase)
	{
		case BoardState::Preflop:
			return Strategies[0];

		case BoardState::Flop:
			return Strategies[1];

		case BoardState::River:
			return Strategies[2];

		case BoardState::Turn:
			return Strategies[3];
	}

	return nullptr;
}

BettingAction DummyOrchastrator::DetermineAction()
{
	CurrentStrategy = DetermineOptimalStrategy();
	return CurrentStrategy->Evaluate();
}
