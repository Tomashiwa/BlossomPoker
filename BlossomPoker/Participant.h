#pragma once
#include <memory>
#include "Player.h"

class Participant
{
public:
	Participant(const std::shared_ptr<Player>& _Owner);
	~Participant();

	void SetRank(unsigned int _Rank) { Rank = _Rank; }
	void UpdateAverageRank(unsigned int _MatchAmt) { AverageRank = (float)Rank / (float)_MatchAmt; }

	void SetMoneyWon(unsigned int _MoneyWon) { MoneyWon = _MoneyWon; }
	void SetMoneyLost(unsigned int _MoneyLost) { MoneyLost = _MoneyLost; }
	void SetHandsWon(unsigned int _HandsWon) { HandsWon = _HandsWon; }
	void SetHandsLost(unsigned int _HandsLost) { HandsLost = _HandsLost; }

	void SetProfits(unsigned int _Profits) { Profits = _Profits; }

	const std::shared_ptr<Player>& GetOwner() { return Owner; }
	unsigned int GetRank() { return Rank; }
	float GetAverageRank() { return AverageRank; }
	float GetFitness() { return Fitness; }
	float GetAverageFitness() { return AverageFitness; }
	
	unsigned int GetMoneyWon() { return MoneyWon; }
	unsigned int GetMoneyLost() { return MoneyLost; }
	unsigned int GetHandsWon() { return HandsWon; }
	unsigned int GetHandsLost() { return HandsLost; }

	int GetProfits() { return Profits; }

	void UpdateFitness();
	void UpdateAverageFitness(unsigned int _MatchAmt);

	void Refresh();

private:
	std::shared_ptr<Player> Owner;

	unsigned int Rank = 0;
	float AverageRank = 0.0f;
	float Fitness = 0.0f;
	float AverageFitness = 0.0f;

	unsigned int MoneyWon = 0;
	unsigned int MoneyLost = 0;
	unsigned int HandsWon = 0;
	unsigned int HandsLost = 0;

	int Profits = 0;
};

