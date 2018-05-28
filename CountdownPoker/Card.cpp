#include "Card.h"

Card::Card(Suit _Suit, Value _Value)
{
	Set(_Suit, _Value);
}


Card::~Card()
{
}

void Card::Set(Suit _Suit, Value _Value)
{
	GivenSuit = _Suit;
	GivenValue = _Value;
}

std::string Card::GetInfo()
{
	std::string Info = "";

	switch (static_cast<int>(GivenSuit)) 
	{
		case 0: Info = "S";
			break;
		case 1: Info = "H";
			break;
		case 2: Info = "C";
			break;
		case 3: Info = "D";
			break;
	}

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
	
	return Info;
}

bool Card::IsGreater(Card* _Comparison)
{
	if (GivenValue == _Comparison->GetValue())
		return GivenSuit > _Comparison->GetSuit() ? true : false;

	return GivenValue > _Comparison->GetValue() ? true : false;
}