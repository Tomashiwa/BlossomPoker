#pragma once
#include "Player.h"
class Folder : public Player
{
public:
	Folder(const std::shared_ptr<Table>& _Table, unsigned int _Index);
	
	BettingAction DetermineAction();
};

