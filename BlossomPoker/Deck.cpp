#include "Card.h"
#include "Deck.h"

Deck::Deck()
{
	Cards.reserve(52);
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
			Cards.push_back(std::make_shared<Card>(static_cast<Suit>(SIndex), static_cast<Rank>(VIndex)));
		}
	}
}

void Deck::Shuffle()
{
	srand(time(0));
	std::random_shuffle(Cards.begin(), Cards.end());
}

bool Deck::Draw(std::shared_ptr<Card>& _NewCard)
{
	_NewCard = std::move(Cards.back());
	Cards.pop_back();
	return Cards.empty();
}

std::shared_ptr<Card> Deck::Draw()
{
	std::shared_ptr<Card> NewCard = std::move(Cards.back());
	Cards.pop_back();
	return NewCard;
}

bool Deck::DrawMulti(unsigned int _Amt, std::vector<std::shared_ptr<Card>>& _NewCards)
{
	for (unsigned int Draws = 0; Draws < _Amt; Draws++)
	{
		std::shared_ptr<Card> NewCard = std::make_shared<Card>();
		Draw(NewCard);
		
		_NewCards.push_back(NewCard);
	}

	return Cards.empty();
}

void Deck::CopyFrom(const std::unique_ptr<Deck>& _Source)
{
	Cards.clear();
	for (auto const& CardToCopy : _Source->Cards)
		Cards.push_back(std::make_shared<Card>(CardToCopy->GetSuit(), CardToCopy->GetRank()));

	/*if (Cards.size() == 0)
	{
		for (auto const& CardToCopy : _Source->Cards)
			Cards.push_back(std::make_shared<Card>(CardToCopy->GetSuit(), CardToCopy->GetRank()));
	}
	else if (Cards.size() == _Source->Cards.size())
	{
		for (unsigned int Index = 0; Index < Cards.size(); Index++)
			Cards[Index]->Set(_Source->Cards[Index]->GetSuit(), _Source->Cards[Index]->GetRank()); 
	}
	else
	{
		std::cout << "UNABLE TO COPY DECK DUE TO SIZE DIFFERENCE \n";
		return;
	}*/
}

void Deck::Print()
{
	std::cout << "Deck's information: \n";
	std::cout << "(Remaining cards): " << Cards.size() << "\n\n";
	
	unsigned int Count = 0;
	for (auto const& C : Cards)//unsigned int CIndex = 0; CIndex < Cards.size(); CIndex++)
	{
		std::cout << C->GetInfo() << "/";
		Count++;

		if (Count >= 13)
		{
			std::cout << "\n";
			Count = 0;
		}
	}

	std::cout << "\n";
}