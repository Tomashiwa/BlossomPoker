#pragma once
#include <array>
#include <vector>
#include <iostream>

#include "Hand.h"
#include "Suit.h"
#include "Value.h"
#include "ComparisonResult.h"

class Card;

class HandEvaluator
{
public:
	HandEvaluator();
	~HandEvaluator();

	int DetermineValue_5Cards(std::array<Card*, 5> _Hand);
	int DetermineValue_7Cards(std::array<Card*, 7> _Hand);

	Hand DetermineType(int _Value);

	ComparisonResult IsBetter5Cards(std::array<Card*, 5> _First, std::array<Card*, 5> _Second);
	ComparisonResult IsBetter7Cards(std::array<Card*, 7> _First, std::array<Card*, 7> _Second);

	std::array<Card*, 5> SortHand(std::array<Card*, 5> _Hand);

	std::array<Card*, 5> GetBestCommunalHand(std::array<Card*, 2> _Hole, std::array<Card*, 5> _Community);

	std::string GetTypeStr(Hand _Hand);
	std::string GetTypeStr(std::array<Card*, 5> _Hand);

	std::string GetStr(std::array<Card*, 5> _Hand);
	std::string GetStr(std::vector<Card*> _Hand);

	void Test();

private:
	int HR[32487834];

	void Initialize();

	int GetCardInt(Card* _Card);
	std::array<int,5> Get5CardsInt(std::array<Card*, 5> _Hand);
	std::array<int,7> Get7CardsInt(std::array<Card*, 7> _Hand);

	std::vector<Card*>::iterator UpperBound(std::vector<Card*>::iterator _First, std::vector<Card*>::iterator _Last, Card* _Current);
	std::vector<Card*> InsertSort(std::vector<Card*> _Cards);
	unsigned int CountCardsWithValue(std::vector<Card*> _Cards, Rank _Value);

};

