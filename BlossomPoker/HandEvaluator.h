#pragma once
#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <random>
#include <time.h>

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

	double DetermineOdds_PreflopHole(std::array<Card*, 2> _Hole);
	double DetermineOdds_MonteCarlo(std::array<Card*, 2> _Hole, std::array<Card*, 5> _Community, int _TrialsAmt);

	int DetermineValue_5Cards(std::array<Card*, 5> _Hand);
	int DetermineValue_7Cards(std::array<Card*, 7> _Hand);
	int DetermineValue_Custom(std::vector<Card*> _Hand);

	Hand DetermineType(int _Value);

	ComparisonResult IsBetter5Cards(std::array<Card*, 5> _First, std::array<Card*, 5> _Second);
	ComparisonResult IsBetter7Cards(std::array<Card*, 7> _First, std::array<Card*, 7> _Second);
	ComparisonResult IsWorse5Cards(std::array<Card*, 5> _First, std::array<Card*, 5> _Second);

	std::array<Card*, 5> SortHand(std::array<Card*, 5> _Hand);

	std::array<Card*, 5> GetBestCommunalHand(std::array<Card*, 2> _Hole, std::array<Card*, 5> _Community);
	std::array<Card*, 5> GetWorstCommunalHand(std::array<Card*, 2> _Hole, std::array<Card*, 5> _Community);
	
	void RandomFill(std::vector<Card*>& _Set, std::vector<Card*> _Dead, int _Target);

	std::string GetTypeStr(Hand _Hand);
	std::string GetTypeStr(std::array<Card*, 5> _Hand);

	std::string GetStr(std::array<Card*, 2> _Hole);
	std::string GetStr(std::array<Card*, 5> _Hand);
	std::string GetStr(std::array<Card*, 7> _Hand);
	std::string GetStr(std::vector<Card*> _Hand);

	void Test();

private:
	int HR[32487834];
	double PreflopOdds[1326];

	std::mt19937 MTGenerator;

	std::array <Card*,52> ReferenceDeck;

	void ConvertPreflopOddsTxt();
	void Initialize();
	
	int GetCardInt(std::string _CardTxt);
	int GetCardInt(Card* _Card);
	std::array<int,5> Get5CardsInt(std::array<Card*, 5> _Hand);
	std::array<int,7> Get7CardsInt(std::array<Card*, 7> _Hand);

	std::vector<Card*>::iterator UpperBound(std::vector<Card*>::iterator _First, std::vector<Card*>::iterator _Last, Card* _Current);
	std::vector<Card*> InsertSort(std::vector<Card*> _Cards);
	unsigned int CountCardsWithValue(std::vector<Card*> _Cards, Rank _Value);
};

