#include "Card.h"

Card::Card()
{

}

Card::Card(Suit _Suit, Rank _Value)
{
	Set(_Suit, _Value);
}

Card::~Card()
{
}

void Card::Set(Suit _Suit, Rank _Value)
{
	GivenSuit = _Suit;
	GivenValue = _Value;

	Info.clear();

	switch (static_cast<int>(GivenValue))
	{
		case 0: Info += "2";
			break;
		case 1: Info += "3";
			break;
		case 2: Info += "4";
			break;
		case 3: Info += "5";
			break;
		case 4: Info += "6";
			break;
		case 5: Info += "7";
			break;
		case 6: Info += "8";
			break;
		case 7: Info += "9";
			break;
		case 8: Info += "10";
			break;
		case 9: Info += "J";
			break;
		case 10: Info += "Q";
			break;
		case 11: Info += "K";
			break;
		case 12: Info += "A";
			break;
	}

	switch (static_cast<int>(GivenSuit))
	{
		case 0: Info += "c";
			break;
		case 1: Info += "d";
			break;
		case 2: Info += "h";
			break;
		case 3: Info += "s";
			break;
	}
}

bool Card::IsGreater(const std::shared_ptr<Card>& _Comparison)
{
	if (GivenValue == _Comparison->GetRank())
		return GivenSuit > _Comparison->GetSuit() ? true : false;

	return GivenValue > _Comparison->GetRank() ? true : false;
}