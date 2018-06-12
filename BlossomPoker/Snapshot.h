#pragma once
#include "Phase.h"
#include "Card.h"
#include <array>

struct Snapshot
{
	Phase Phase;

	unsigned int Pot;
	unsigned int Stack;
	unsigned int Contribution;
	unsigned int RequiredAnte;
	unsigned int CurrentAnte;

	std::array<Card*, 2> Hole;
	std::array<Card*, 5> Communal;
};