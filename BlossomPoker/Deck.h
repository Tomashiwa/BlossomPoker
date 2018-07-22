#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <time.h>

class Card;

class Deck
{
public:
	Deck();
	~Deck();

	void Refill();
	void Shuffle();
	
	Card* Draw();
	std::vector<Card*> DrawMultiple(unsigned int _Amt);

	void Remove(Card* _Target);

	void Print();
	std::vector<Card*> GetCards() { return Cards; }

private:
	std::vector<Card*> Cards;

};

