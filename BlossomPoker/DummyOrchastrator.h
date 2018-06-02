#pragma once
#include "Snapshot.h"

class DummyAI;
class DummyStrategy;

class DummyOrchastrator
{
public:
	DummyOrchastrator(DummyAI* _AI);
	~DummyOrchastrator();

	DummyStrategy* DetermineOptimalStrategy();
	BettingAction DetermineAction();

	DummyAI* GetAI() { return AI; }

private:
	DummyAI* AI;

	std::array<DummyStrategy*, 4> Strategies;
	DummyStrategy* CurrentStrategy;
};

