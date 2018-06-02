#pragma once
#include "BoardState.h"
#include "Card.h"
#include <array>

struct Snapshot
{
	BoardState Phase;

	unsigned int Pot;
	unsigned int Stack;
	unsigned int Contribution;
	unsigned int RequiredAnte;
	unsigned int CurrentAnte;

	std::array<Card*, 2> Hole;
	std::array<Card*, 5> Communal;
};