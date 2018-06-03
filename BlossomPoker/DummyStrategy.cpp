#include "DummyStrategy.h"
#include "DummyAI.h"
#include "DummyOrchastrator.h"

DummyStrategy::DummyStrategy(DummyOrchastrator* _Orchastrator)
{
}

DummyStrategy::~DummyStrategy()
{
}

BettingAction DummyStrategy::Evaluate()
{
	Snapshot Situation = Orchastrator->GetAI()->GetSnapshot();
	std::vector<BettingAction> Actions = Orchastrator->GetAI()->GetAvaliableActions();
	
	//Evaluate the probability of winning w/ player's current hand


	return BettingAction::NONE;
}
