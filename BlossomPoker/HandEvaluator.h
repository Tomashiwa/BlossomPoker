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
#include <memory>

#include "Hand.h"
#include "Suit.h"
#include "Rank.h"
#include "ComparisonResult.h"

class Card;

class HandEvaluator
{
public:
	HandEvaluator();
	~HandEvaluator();

	float DetermineOdds_PreflopHole(std::array<std::shared_ptr<Card>, 2> _Hole);
	float DetermineOdds_MonteCarlo(std::array<std::shared_ptr<Card>, 2> _Hole, std::array<std::shared_ptr<Card>, 5> _Community, unsigned int _TrialsAmt);
	float DetermineOdds_MonteCarlo_Multi(std::array<std::shared_ptr<Card>, 2> _Hole, std::array<std::shared_ptr<Card>, 5> _Community, unsigned int _PlayerAmt, unsigned int _TrialsAmt);

	int DetermineValue_5Cards(const std::array<std::shared_ptr<Card>, 5>& _Hand);
	int DetermineValue_7Cards(const std::array<std::shared_ptr<Card>, 7>& _Hand);
	int DetermineValue_Custom(const std::vector<std::shared_ptr<Card>>& _Hand);

	Hand DetermineType(int _Value);

	ComparisonResult IsBetter5Cards(std::array<std::shared_ptr<Card>, 5> _First, std::array<std::shared_ptr<Card>, 5> _Second);
	ComparisonResult IsBetter7Cards(std::array<std::shared_ptr<Card>, 7> _First, std::array<std::shared_ptr<Card>, 7> _Second);
	ComparisonResult IsWorse5Cards(std::array<std::shared_ptr<Card>, 5> _First, std::array<std::shared_ptr<Card>, 5> _Second);

	std::array<std::shared_ptr<Card>, 5> SortHand(std::array<std::shared_ptr<Card>, 5> _Hand);

	std::array<std::shared_ptr<Card>, 5> GetBestCommunalHand(std::array<std::shared_ptr<Card>, 2> _Hole, std::array<std::shared_ptr<Card>, 5> _Community);
	std::array<std::shared_ptr<Card>, 5> GetWorstCommunalHand(std::array<std::shared_ptr<Card>, 2> _Hole, std::array<std::shared_ptr<Card>, 5> _Community);
	
	void RandomFill(std::vector<std::shared_ptr<Card>>& _Set, std::vector<std::shared_ptr<Card>>& _Dead, unsigned int _Target);

	std::string GetTypeStr(Hand _Hand);
	std::string GetTypeStr(std::array<std::shared_ptr<Card>, 5> _Hand);

	std::string GetStr(std::array<std::shared_ptr<Card>, 2> _Hole);
	std::string GetStr(std::array<std::shared_ptr<Card>, 5> _Hand);
	std::string GetStr(std::array<std::shared_ptr<Card>, 7> _Hand);
	std::string GetStr(std::vector<std::shared_ptr<Card>> _Hand);

	void Test();

private:
	int HR[32487834];
	float PreflopOdds[1326];

	std::mt19937 MTGenerator;

	std::array <std::shared_ptr<Card>,52> ReferenceDeck;

	void ConvertPreflopOddsTxt();
	void Initialize();
	
	int GetCardInt(std::string _CardTxt);
	int GetCardInt(const std::shared_ptr<Card>& _Card);
	void Get5CardsInt(const std::array<std::shared_ptr<Card>, 5>& _Hand, std::array<int,5>& _CardInts);
	void Get7CardsInt(const std::array<std::shared_ptr<Card>, 7>& _Hand, std::array<int,7>& _CardInts);

	std::vector<std::shared_ptr<Card>>::iterator UpperBound(std::vector<std::shared_ptr<Card>>::iterator _First, std::vector<std::shared_ptr<Card>>::iterator _Last, std::shared_ptr<Card> _Current);
	void InsertSort(std::vector<std::shared_ptr<Card>>& _Cards);
	unsigned int CountCardsWithValue(std::vector<std::shared_ptr<Card>> _Cards, Rank _Value);
};

