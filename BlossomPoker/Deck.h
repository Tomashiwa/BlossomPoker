#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include <time.h>

class Card;

class Deck
{
public:
	Deck();

	Deck(const Deck&) = delete;
	Deck& operator= (const Deck&) = delete;

	~Deck();

	void Refill();
	void Shuffle();
	
	std::shared_ptr<Card> Draw();
	bool Draw(std::shared_ptr<Card>& _NewCard);
	bool DrawMulti(unsigned int _Amt, std::vector<std::shared_ptr<Card>>& _NewCards);//std::vector<std::shared_ptr<Card>> DrawMultiple(unsigned int _Amt);

	void CopyFrom(const std::unique_ptr<Deck>& _Source);

	void Print();
	void GetCards(std::vector<std::shared_ptr<Card>>& _Cards) { _Cards = Cards; }

private:
	std::vector<std::shared_ptr<Card>> Cards;

};

