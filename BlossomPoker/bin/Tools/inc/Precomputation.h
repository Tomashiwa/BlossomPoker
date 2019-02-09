#pragma once

#include <memory>
#include <array>
#include <fstream>
#include <sstream>

#include "../../Cards/inc/Card.h"
#include "../../Cards/inc/HandEvaluator.h"
#include "../../../OMPEval/inc/Evaluator.h"

class Precomputation
{
public:
	Precomputation(const std::shared_ptr<HandEvaluator>& _Evaluator);
	~Precomputation();

	void ComputePreflopOdds(unsigned int _OppoAmt, unsigned int _Trials);

private:
	std::vector<Card> ReferenceDeck;
	std::shared_ptr<HandEvaluator> Evaluator;
};

