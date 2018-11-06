#pragma once
#include <memory>

class Player;

struct Participant
{
	std::shared_ptr<Player> Owner;
	unsigned int Rank = 0;
	float Fitness = 0.0;
	unsigned int HandsParticipated = 0;

	unsigned int MoneyWon = 0;
	unsigned int MoneyLost = 0;
	unsigned int HandsWon = 0;
	unsigned int HandsLost = 0;

	int Profits = 0;

	Participant(const std::shared_ptr<Player>& _Owner) : Owner(_Owner)
	{};

	void UpdateFitness()
	{
		Fitness = ((float)MoneyWon / (HandsWon == 0 ? (float) 1 : (float) HandsWon)) - ((float)MoneyLost / (HandsLost == 0 ? (float)1 : (float)HandsLost));
	}
};