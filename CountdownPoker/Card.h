#pragma once
#include <string>
#include "Suits.h"
#include "Values.h"

class Card
{
public:
	Card(Suit _Suit, Value _Value);
	~Card();
	
	Suit GetSuit() { return GivenSuit; };
	Value GetValue() { return GivenValue; };
	int GetValueInt() { return static_cast<int>(GivenValue); }

	void Set(Suit _Suit, Value _Value);

	std::string GetInfo();
	bool IsGreater(Card* _Comparison);

private:
	Suit GivenSuit;
	Value GivenValue;
};

