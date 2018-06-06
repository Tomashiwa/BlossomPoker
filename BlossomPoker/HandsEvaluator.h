#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <array>

#include "Hands.h"
#include "Card.h"
#include "ComparisonResult.h"

namespace HandsEvaluator
{
	Card* LowestReference();
	Card* HighestReference();

	std::vector<Card*>::iterator UpperBound(std::vector<Card*>::iterator _First, std::vector<Card*>::iterator _Last, Card* _Current);

	std::vector<Card*> InsertSort(std::vector<Card*> _Cards);
	std::array<Card*,5> SortHand(std::array<Card*,5> _Hand);

	bool IsFlush(std::vector<Card*> _Hand);
	bool IsStraight(std::vector<Card*> _Hand);
	
	Hand DetermineHandType(std::array<Card*,5> _Hand);
	Hand DetermineHandType(std::vector<Card*> _Hand);

	unsigned int CountCardsWithValue(std::vector<Card*> _Hand, Value _Value);

	ComparisonResult CompareHand(std::array<Card*, 5> _First, std::array<Card*, 5> _Second);
	std::array<Card*, 5> GetBestCommunalHand(std::array<Card*, 2> _Hole, std::array<Card*, 5> _Communal);

	std::string GetHandTypeStr(Hand _Type);

	std::string GetHandStr(std::array<Card*, 5> _Hand);
	std::string GetCardsStr(std::vector<Card*> _Hand);

	
}