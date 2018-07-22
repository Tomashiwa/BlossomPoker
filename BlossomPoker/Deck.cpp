#include "Card.h"
#include "Deck.h"

Deck::Deck()
{
	Refill();
	Shuffle();
}

Deck::~Deck()
{
}

void Deck::Refill()
{
	Cards.clear();
	
	for (unsigned int SIndex = 0; SIndex < 4; SIndex++)
	{
		for (unsigned int VIndex = 0; VIndex < 13; VIndex++)
		{
			Suit NewSuit = static_cast<Suit>(SIndex);
			Rank NewValue = static_cast<Rank>(VIndex);

			Cards.push_back(new Card(NewSuit, NewValue));
		}
	}
}

void Deck::Shuffle()
{
	srand(time(0));
	std::random_shuffle(Cards.begin(), Cards.end());
}

Card* Deck::Draw()
{
	Card* DrawnCard = Cards.back();
	Cards.pop_back();
	return DrawnCard;
}

std::vector<Card*> Deck::DrawMultiple(unsigned int _Amt)
{
	std::vector<Card*> DrawnCards;

	for (unsigned int Draws = 0; Draws < _Amt; Draws++)
		DrawnCards.push_back(Draw());

	return DrawnCards;
}

void Deck::Remove(Card* _Target)
{
	for (unsigned int Index = 0; Index < Cards.size(); Index++)
	{
		if (Cards[Index]->GetInfo() == _Target->GetInfo())
			Cards.erase(Cards.begin() + Index);
	}
}

void Deck::Print()
{
	std::cout << "Deck's information: " << std::endl;
	std::cout << "(Remaining cards): " << Cards.size() << std::endl << std::endl;
	
	unsigned int Count = 0;
	for (unsigned int CIndex = 0; CIndex < Cards.size(); CIndex++)
	{
		std::cout << Cards[CIndex]->GetInfo();

		Count++;
		std::cout << "/";
		if (Count >= 13)
		{
			std::cout << std::endl;
			Count = 0;
		}
	}

	std::cout << std::endl;
}