#pragma once
#include <memory>
#include "../../Player/inc/Player.h"

class Participant
{
public:
	Participant(const std::shared_ptr<Player>& _Owner);
	~Participant();

	void SetOwner(const std::shared_ptr<Player>& _Player) { Owner = _Player; }

	void SetMoneyWon(unsigned int _MoneyWon) { MoneyWon = _MoneyWon; }
	void SetMoneyLost(unsigned int _MoneyLost) { MoneyLost = _MoneyLost; }
	void SetHandsWon(unsigned int _HandsWon) { HandsWon = _HandsWon; }
	void SetHandsLost(unsigned int _HandsLost) { HandsLost = _HandsLost; }

	void SetProfits(unsigned int _Profits) { Profits = _Profits; }

	const std::shared_ptr<Player>& GetOwner() { return Owner; }
	float GetFitness() { return Fitness; }
	
	unsigned int GetMoneyWon() { return MoneyWon; }
	unsigned int GetMoneyLost() { return MoneyLost; }
	unsigned int GetHandsWon() { return HandsWon; }
	unsigned int GetHandsLost() { return HandsLost; }

	int GetProfits() { return Profits; }

	void UpdateFitness();

	void Refresh();

private:
	std::shared_ptr<Player> Owner;

	float Fitness = 0.0f;

	unsigned int MoneyWon = 0;
	unsigned int MoneyLost = 0;
	unsigned int HandsWon = 0;
	unsigned int HandsLost = 0;

	int Profits = 0;
};

