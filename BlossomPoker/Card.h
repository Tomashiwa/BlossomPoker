#pragma once

#include <string>

enum class Suit { Club, Diamond, Heart, Spade };
enum class Rank { Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace };

class Card
{
public:
	Card() = default;

	Card(Suit _Suit, Rank _Rank) : GivenSuit(_Suit), GivenRank(_Rank) {};
	Card(std::string _Info);

	Suit Get_Suit() const { return GivenSuit; };
	Rank Get_Rank() const { return GivenRank; };
	
	bool operator== (const Card& _Other) const
	{
		if (GivenRank == _Other.GivenRank)
			return GivenSuit > _Other.GivenSuit;

		return GivenRank > _Other.GivenRank;
	}

	bool operator>(const Card& _Other) const
	{
		return GivenSuit == _Other.GivenSuit && GivenRank == _Other.GivenRank;
	}

	std::string To_String() const;

	explicit operator int()
	{
		return (static_cast<int>(GivenRank) * 4) + static_cast<int>(GivenSuit) + 1;
	}

private:
	Suit GivenSuit;
	Rank GivenRank;
};

