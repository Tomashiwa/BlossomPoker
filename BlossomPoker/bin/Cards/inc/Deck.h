#pragma once

#include <vector>
#include <memory>
#include <algorithm>

class Card;

class Deck
{
public:
	Deck();

	void Refill();
	void Shuffle();
	
	Card Draw();
	std::vector<Card> DrawMulti(std::size_t _Amt);

	void CopyFrom(Deck& _Source);

	void Print() const;

	std::vector<Card> GetCards() const { return Cards; }

private:
	std::vector<Card> Cards;

};

