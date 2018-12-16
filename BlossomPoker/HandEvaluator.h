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

#include "Card.h"
#include "Hand.h"
#include "ComparisonResult.h"

class HandEvaluator
{
public:
	HandEvaluator();

	float DetermineOdds_MonteCarlo_Multi(std::array<Card, 2> _Hole, std::vector<Card> _Community, unsigned int _PlayerAmt, unsigned int _TrialsAmt);

	int DetermineValue_5Cards(const std::array<Card, 5>& _Hand);
	int DetermineValue_7Cards(const std::array<Card, 7>& _Hand);
	
	Hand DetermineType(int _Value);

	ComparisonResult IsBetter5Cards(std::array<Card, 5> _First, std::array<Card, 5> _Second);
	ComparisonResult IsBetter7Cards(std::array<Card, 7> _First, std::array<Card, 7> _Second);
	ComparisonResult IsWorse5Cards(std::array<Card, 5> _First, std::array<Card, 5> _Second);

	std::array<Card, 5> GetBestCommunalHand(std::array<Card, 2> _Hole, std::vector<Card> _Community);
	
	void RandomFill(std::vector<Card>& _Set, std::vector<Card>& _Dead, std::size_t _Target);

	std::string GetTypeStr(Hand _Hand);
	std::string GetTypeStr(std::array<Card, 5> _Hand);

	std::string GetStr(std::array<Card, 2> _Hole);
	std::string GetStr(std::array<Card, 5> _Hand);
	std::string GetStr(std::array<Card, 7> _Hand);
	std::string GetStr(std::vector<Card> _Hand);

private:
	int HR[32487834];

	std::mt19937 MTGenerator;

	std::vector<Card> ReferenceDeck;//std::array <Card,52> ReferenceDeck;

	void Initialize();
	
	int GetCardInt(Card& _Card);
	std::array<int, 5> Get5CardsInt(const std::array<Card, 5>& _Hand);
	std::array<int, 7> Get7CardsInt(const std::array<Card, 7>& _Hand);
};

