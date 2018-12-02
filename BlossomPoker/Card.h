#pragma once
#include <memory>
#include <string>

#include "Suit.h"
#include "Rank.h"

class Card
{
public:
	Card();
	Card(Suit _Suit, Rank _Value);

	Card(const Card&) = delete;

	~Card();
	
	Suit GetSuit() { return GivenSuit; };
	Rank GetRank() { return GivenValue; };
	
	int GetSuitInt() { return static_cast<int>(GivenSuit); }
	int GetRankInt() { return static_cast<int>(GivenValue); }

	void Set(Suit _Suit, Rank _Value);
	bool IsEqualTo(const std::shared_ptr<Card>& _Comparison);
	bool IsGreater(const std::shared_ptr<Card>& _Comparison);

	std::string GetInfo() { return Info; }

private:
	Suit GivenSuit;
	Rank GivenValue;
	std::string Info;
};

