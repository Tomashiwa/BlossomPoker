#pragma once
#include "BettingAction.h"
#include "Snapshot.h"

class BlossomAI;
class Strategy;

class Orchastrator
{
public:
	Orchastrator(BlossomAI* _AI);
	~Orchastrator();

	Strategy* DetermineOptimalStrategy();
	BettingAction DetermineAction();
	
	BlossomAI* GetAI() { return AI; }

private:
	BlossomAI* AI;

	Strategy* CurrentStrategy;
	std::array<Strategy*, 4> Strategies;
};

