#pragma once
#include "Player.h"

class BlossomPlayer : public Player
{
public:
	BlossomPlayer(const std::shared_ptr<Table>& _Table, const std::shared_ptr<HandEvaluator>& _Evaluator, unsigned int _Index);

	BettingAction DetermineAction();

	BlossomAI& GetAI() { return AI; }

private:
	BlossomAI AI;
};

