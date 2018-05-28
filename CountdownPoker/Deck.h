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

	void Print();

private:
	std::vector<Card*> Cards;

};

