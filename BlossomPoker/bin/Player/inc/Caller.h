#pragma once
#include "Player.h"
class Caller : public Player
{
public:
	Caller(const std::shared_ptr<Table>& _Table, unsigned int _Index);
	
	BettingAction DetermineAction();
};

