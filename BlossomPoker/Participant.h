#pragma once
#include <memory>

#include "Player.h"

class HandEvaluator;

struct Participant
{
	std::shared_ptr<Player> Owner;
	unsigned int Rank = 0;
	float Fitness = 0.0;
	float AverageFitness = 0.0;

	unsigned int HandsParticipated = 0;

	unsigned int MoneyWon = 0;
	unsigned int MoneyLost = 0;
	unsigned int HandsWon = 0;
	unsigned int HandsLost = 0;

	int Profits = 0;

	Participant(const std::shared_ptr<Player>& _Owner, const std::shared_ptr<HandEvaluator>& _Evaluator) : Owner(_Owner)
	{
		Owner->GetAI().SetEvalutor(_Evaluator);
	};

	void UpdateFitness()
	{
		Fitness = ((float)MoneyWon / (HandsWon == 0 ? (float) 1 : (float) HandsWon)) - ((float)MoneyLost / (HandsLost == 0 ? (float)1 : (float)HandsLost));
	}

	void UpdateAverageFitness(unsigned int _MatchAmt)
	{
		AverageFitness = Fitness / (float)_MatchAmt;
	}

	void Refresh()
	{
		Rank = 0;
		Fitness = 0.0;
		AverageFitness = 0.0;

		HandsParticipated = 0;

		MoneyWon = 0;
		MoneyLost = 0;
		HandsWon = 0;
		HandsLost = 0;
		
		Profits = 0;
	}
};