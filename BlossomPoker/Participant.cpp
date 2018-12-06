#include "Participant.h"

Participant::Participant(const std::shared_ptr<Player>& _Owner) : Owner(_Owner)
{}


Participant::~Participant()
{}

void Participant::UpdateFitness()
{
	Fitness = ((float)MoneyWon / (HandsWon == 0 ? (float)1 : (float)HandsWon)) - ((float)MoneyLost / (HandsLost == 0 ? (float)1 : (float)HandsLost));
}

void Participant::Refresh()
{
	Rank = 0;
	Fitness = 0.0;

	MoneyWon = 0;
	MoneyLost = 0;
	HandsWon = 0;
	HandsLost = 0;

	Profits = 0;
}
