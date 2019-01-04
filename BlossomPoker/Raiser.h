#pragma once
#include <random>
#include <chrono>
#include "Player.h"

class Raiser : public Player
{
public:
	Raiser(const std::shared_ptr<Table>& _Table, unsigned int _Index);

	BettingAction DetermineAction();

	unsigned int GetRaiseBetAmt() { return RaiseBetAmt; }

private:
	std::mt19937 MTGenerator;
	unsigned int RaiseBetAmt = 0;
};

