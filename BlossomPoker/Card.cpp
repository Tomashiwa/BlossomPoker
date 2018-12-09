#include "Card.h"

Card::Card(std::string _Text)
{
	int SuitInt = 0, RankInt = 0;

	char SuitType = _Text.back();
	std::string RankType = _Text.substr(0, _Text.size() - 1);

	switch (SuitType)
	{
		case 's':
			SuitInt = static_cast<int>(Suit::Spade);
			break;
		case 'h':
			SuitInt = static_cast<int>(Suit::Heart);
			break;
		case 'd':
			SuitInt = static_cast<int>(Suit::Diamond);
			break;
		case 'c':
			SuitInt = static_cast<int>(Suit::Club);
			break;
		default:
			SuitInt = -1;
			break;
	}

	if (RankType == "J")
		RankInt = static_cast<int>(Rank::Jack);
	else if (RankType == "Q")
		RankInt = static_cast<int>(Rank::Queen);
	else if (RankType == "K")
		RankInt = static_cast<int>(Rank::King);
	else if (RankType == "A")
		RankInt = static_cast<int>(Rank::Ace);
	else
		RankInt = std::stoi(RankType) - 2;

	if (SuitInt == -1 || RankInt == -1)
		throw std::invalid_argument("Invalid card text during construction...");

	GivenSuit = static_cast<Suit>(SuitInt);
	GivenRank = static_cast<Rank>(RankInt);
}

std::string Card::To_String() const
{
	std::string Info;

	switch (static_cast<int>(GivenRank))
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

	return Info;
}