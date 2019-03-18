#pragma once

#include <memory>
#include <array>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "../../Cards/inc/Card.h"
#include "../../Cards/inc/HandEvaluator.h"
#include "../../../OMPEval/inc/Evaluator.h"
#include "../../Table/inc/Phase.h"

class Precomputation
{
public:
	Precomputation(const std::shared_ptr<HandEvaluator>& _Evaluator);
	~Precomputation();

	void ComputePreflopOdds(unsigned int _OppoAmt, unsigned int _Trials);
	void ComputeFlopOdds(unsigned int _OppoAmt, unsigned int _Trials);

private:
	std::vector<Card> ReferenceDeck;
	std::shared_ptr<HandEvaluator> Evaluator;

	void FillStraightGap(std::vector<Card>& _Current, Rank _Min, Rank _Max);

	bool AreCardsIdentical(std::vector<Card> _First, std::vector<Card> _Second);
	bool AreCardsIdentical_Order(std::vector<Card> _First, std::vector<Card> _Second);
	bool AreHandsIdentical(std::vector<Card> _First, std::vector<Card> _Second);

	bool IsCardInHand(Card _Card, std::vector<Card> _Hand);
	bool IsCardInHand(Rank _Rank, Suit _Suit, std::vector<Card> _Hand);

	bool IsCardInStraightRange(Card _Card, std::vector<Card> _Straight);

	bool IsStraightValid(std::vector<Card> _Straight);
	bool IsFlushValid(std::vector<Card> _Flush);

	void ComputeOdd(std::vector<Card> _Hand, unsigned int _MaxOppoAmt, unsigned int _Trials, std::ofstream& _File);
};

