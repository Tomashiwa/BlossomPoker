#pragma once
#include <array>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <random>
#include <time.h>
#include <memory>
#include <unordered_map>

#include "Card.h"
#include "ComparisonResult.h"
#include "../../../OMPEval/inc/Evaluator.h"

enum class Hand { High, Pair, TwoPair, ThreeKind, Straight, Flush, FullHouse, FourKind, StraightFlush, RoyalFlush };

struct CardCombo
{
	std::array<Card, 2> Hole;
	std::vector<Card> Community;

	CardCombo() {};
	CardCombo(std::array<Card, 2> _Hole, std::vector<Card> _Community) : Hole(_Hole), Community(_Community) {};

	bool operator==(const CardCombo& _ComparedTo) const
	{
		if (Hole.size() != _ComparedTo.Hole.size() || Community.size() != _ComparedTo.Community.size() || Hole.size() == 0)
			return false;

		for (auto const& CurrentCard : Hole)
		{
			if (std::find_if(_ComparedTo.Hole.begin(), _ComparedTo.Hole.end(), [&](const Card& _Card) { return CurrentCard == _Card; }) == _ComparedTo.Hole.end())
				return false;
		}

		for (auto const& CurrentCard : Community)
		{
			if (std::find_if(_ComparedTo.Community.begin(), _ComparedTo.Community.end(), [&](const Card& _Card) { return CurrentCard == _Card; }) == _ComparedTo.Community.end())
				return false;
		}

		return true;
	}
};

namespace std
{
	template<>
	struct hash<CardCombo>
	{
		std::size_t operator()(const CardCombo& _ComparedTo) const
		{
			std::size_t Hash = 0;

			for (auto const& Card : _ComparedTo.Hole)
			{
				unsigned int HashedCard = static_cast<int>(Card);
				HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
				HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
				HashedCard = (HashedCard >> 16) ^ HashedCard;

				Hash += HashedCard;
			}

			for (auto const& Card : _ComparedTo.Community)
			{
				unsigned int HashedCard = static_cast<int>(Card);
				HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
				HashedCard = ((HashedCard >> 16) ^ HashedCard) * 0x45d9f3b;
				HashedCard = (HashedCard >> 16) ^ HashedCard;

				Hash += HashedCard;
			}

			return Hash;
		}
	};
}

class HandEvaluator
{
public:
	HandEvaluator();

	float DetermineOdds_Preflop(std::array<Card, 2> _Hole, unsigned int _OppoAmt);
	float DetermineOdds_Flop(std::array<Card, 2> _Hole, std::vector<Card> _Community, unsigned int _OppoAmt);
	float DetermineOdds_MonteCarlo_Multi_TwoPlusTwo(std::array<Card, 2> _Hole, std::vector<Card> _Community, unsigned int _OppoAmt, unsigned int _TrialsAmt);
	float DetermineOdds_MonteCarlo_Multi_OMPEval(std::array<Card, 2> _Hole, std::vector<Card> _Community, unsigned int _OppoAmt, unsigned int _TrialsAmt);

	int DetermineValue_Cards(const std::vector<Card>& _Cards);
	int DetermineValue_5Cards_TwoPlusTwo(const std::array<Card, 5>& _Hand);
	int DetermineValue_5Cards_OMPEval(const std::array<Card, 5>& _Hand);
	int DetermineValue_7Cards_TwoPlusTwo(const std::array<Card, 7>& _Hand, int _PrecomputeScore, unsigned int _ContinueFrom);
	int DetermineValue_7Cards_OMPEval(const std::array<Card, 7>& _Hand);

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

	Card GetCardFromStr(std::string _Text);

private:
	int HR[32487834];

	std::mt19937 MTGenerator;
	std::unique_ptr<omp::Evaluator> Eval;

	std::vector<Card> ReferenceDeck;

	std::unordered_map<CardCombo, std::vector<float>> PreflopOdds;
	std::unordered_map<CardCombo, std::vector<float>> FlopOdds;

	void Initialize();
	
	int GetCardInt_TwoPlusTwo(Card& _Card);
	int GetCardInt_OMPEval(Suit _Suit, Rank _Rank);
	std::array<int, 5> Get5CardsInt_TwoPlusTwo(const std::array<Card, 5>& _Hand);
	std::array<int, 7> Get7CardsInt_TwoPlusTwo(const std::array<Card, 7>& _Hand);
	std::array<int, 7> Get7CardsInt_OMPEval(const std::array<Card, 7>& _Hand);
};

