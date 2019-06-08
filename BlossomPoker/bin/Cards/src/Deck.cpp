#include <random>
#include <iostream>
#include <algorithm>

#include "../inc/Card.h"
#include "../inc/Deck.h"
#include "../../Tools/inc/PRNGlib.h"

Deck::Deck()
{
	Cards.reserve(52);
	Refill();
	Shuffle();
}

void Deck::Refill()
{
	Cards.clear();
	
	for (unsigned int SIndex = 0; SIndex < 4; SIndex++)
	{
		for (unsigned int RIndex = 0; RIndex < 13; RIndex++)
		{
			Cards.emplace_back(static_cast<Suit>(SIndex), static_cast<Rank>(RIndex));
		}
	}
}

void Deck::Shuffle()
{
	for (unsigned int Index = Cards.size(); Index > 1; Index--)
	{
		unsigned int RandIndex = PRNGlib::xoroshiro128::next() % 51;
		std::swap(Cards[Index - 1], Cards[RandIndex]);
	}

	//std::mt19937_64 Twister{ std::random_device{}() };
	//std::shuffle(Cards.begin(), Cards.end(), Twister);
}

Card Deck::Draw()
{
	Card Result = Cards.back();
	Cards.pop_back();
	return Result;
}

std::vector<Card> Deck::DrawMulti(std::size_t _Amt)
{
	return{ Cards.rbegin(), std::next(Cards.rbegin(), _Amt) };
}

void Deck::CopyFrom(Deck& _Source)
{
	Cards.clear();
	for (auto const& CardToCopy : _Source.Cards)
		Cards.emplace_back(CardToCopy.Get_Suit(), CardToCopy.Get_Rank());
}
 
void Deck::Print() const
{
	std::cout << "Deck's information: \n";
	std::cout << "(Remaining cards): " << Cards.size() << "\n\n";
	
	unsigned int Count = 0;
	for (auto const& Card : Cards)
	{
		std::cout << Card.To_String() << "/";
		Count++;

		if (Count >= 13)
		{
			std::cout << "\n";
			Count = 0;
		}
	}

	std::cout << "\n";
}