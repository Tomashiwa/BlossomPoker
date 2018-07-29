#pragma once
#include "Phase.h"
#include "BettingAction.h"
#include "Card.h"
#include <array>
#include <vector>

struct Snapshot
{
	Phase Phase;

	unsigned int PlayerAmt;
	unsigned int Pot;
	unsigned int BB;
	unsigned int Stack;
	unsigned int Contribution;
	unsigned int RequiredAnte;
	unsigned int CurrentAnte;

	std::array<Card*, 2> Hole;
	std::array<Card*, 5> Communal;

	std::vector<BettingAction> AvaliableActions;
};