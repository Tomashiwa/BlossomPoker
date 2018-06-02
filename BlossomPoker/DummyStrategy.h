#pragma once
#include "BettingAction.h"
#include <vector>

class DummyAI;
class DummyOrchastrator;

class DummyStrategy
{
public:
	DummyStrategy(DummyOrchastrator* _Orchastrator);
	~DummyStrategy();

	BettingAction Evaluate();

private:
	DummyOrchastrator* Orchastrator;
};

