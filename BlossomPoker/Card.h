#pragma once
#include <string>

#include "Suit.h"
#include "Value.h"

class Card
{
public:
	Card(Suit _Suit, Rank _Value);
	~Card();
	
	Suit GetSuit() { return GivenSuit; };
	Rank GetRank() { return GivenValue; };
	int GetSuitInt() { return static_cast<int>(GivenSuit); }
	int GetRankInt() { return static_cast<int>(GivenValue); }

	void Set(Suit _Suit, Rank _Value);

	std::string GetInfo();
	bool IsGreater(Card* _Comparison);

private:
	Suit GivenSuit;
	Rank GivenValue;
};

