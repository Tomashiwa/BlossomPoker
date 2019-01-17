#pragma once
#include <array>
#include <vector>

#include "../../Table/inc/Phase.h"
#include "../../Table/inc/BettingAction.h"
#include "../../Cards/inc/Card.h"

struct Snapshot
{
	unsigned int Rounds;
	Phase Phase;

	unsigned int PlayerAmt;
	unsigned int Pot;
	unsigned int BB;
	unsigned int Stack;
	unsigned int Contribution;
	unsigned int RequiredAnte;
	unsigned int CurrentAnte;
	unsigned int PrevRaiseBet;

	std::array<Card,2> Hole;
	std::vector<Card> Communal;

	std::vector<BettingAction> AvaliableActions;
};