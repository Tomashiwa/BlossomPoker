#include "Participant.h"

Participant::Participant(const std::shared_ptr<Player>& _Owner) : Owner(_Owner)
{}


Participant::~Participant()
{}

void Participant::UpdateFitness()
{
	//Profit per Hand = (MoneyWon - MoneyLost) / (HandsWon + HandsLost)
	Fitness = ((float) MoneyWon - (float) MoneyLost) / (float)(HandsWon + HandsLost);

	//Profit per Hand = (MoneyWon / HandsWon) - (MoneyLost / HandsLost)
	//Fitness = ((float)MoneyWon / (HandsWon == 0 ? (float)1 : (float)HandsWon)) - ((float)MoneyLost / (HandsLost == 0 ? (float)1 : (float)HandsLost));
}

void Participant::Refresh()
{
	Fitness = 0.0f;

	MoneyWon = 0;
	MoneyLost = 0;
	HandsWon = 0;
	HandsLost = 0;

	Profits = 0;
}
