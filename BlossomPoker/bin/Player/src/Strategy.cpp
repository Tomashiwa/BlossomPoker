#include "../inc/Strategy.h"
#include "../../AI/inc/BlossomAI.h"

Strategy::Strategy(Phase _Corresponding)
{
	CorrespondingPhase = _Corresponding;

	for (unsigned int Index = 0; Index < Thresholds.size(); Index++)
		Thresholds[Index] = 0.0;
}

Strategy::~Strategy()
{
}

BettingAction Strategy::DetermineAction(std::vector<BettingAction> _AvaliableActions, float _WinRate, unsigned int _PlayerAmt)
{
	if (_AvaliableActions.size() == 0)
	{
		std::cout << "No avaliable actions???\n";
		return BettingAction::NONE;
	}
	else if (_AvaliableActions.size() == 1)
	{
		std::cout << "Only avaliable action is to Fold\n";
		return BettingAction::Fold;
	}
	else if (_AvaliableActions.size() == 2 && _AvaliableActions[1] == BettingAction::Check)
	{
		std::cout << "Only avaliable action other than Fold is Check\n";
		return _AvaliableActions[1];
	}

	//std::cout << "Thresholds: " << Thresholds[0] << "," << Thresholds[1] << "," << Thresholds[2] << "," << Thresholds[3] << "\n";
	CalculateRequirements(_PlayerAmt);
	//std::cout << "\n";
	//std::cout << "Win Rate: " << _WinRate << " / Call: " << Requirements[0] << ", MinRaise: " << Requirements[1] << ", HalfPot: " << Requirements[2] << ", FullPot: " << Requirements[3] << " (Players Left: " << _PlayerAmt << ")\n";

	if (_WinRate >= Requirements[3])
		Sizing = RaiseBetSize::Pot;
	else if (_WinRate >= Requirements[2])
		Sizing = RaiseBetSize::HalfPot;
	else if (_WinRate >= Requirements[1])
		Sizing = RaiseBetSize::Minimum;
	else
		Sizing = RaiseBetSize::NONE;

	if (Sizing != RaiseBetSize::NONE)
	{
		if (std::find(_AvaliableActions.begin(), _AvaliableActions.end(), BettingAction::Bet) != _AvaliableActions.end())
		{
			//std::cout << "Action taken is BET\n";
			return BettingAction::Bet;
		}
		else if (std::find(_AvaliableActions.begin(), _AvaliableActions.end(), BettingAction::Raise) != _AvaliableActions.end())
		{
			//std::cout << "Action taken is RAISE\n";
			return BettingAction::Raise;
		}
	}

	if (_WinRate >= Requirements[0] && std::find(_AvaliableActions.begin(), _AvaliableActions.end(), BettingAction::Call) != _AvaliableActions.end())
		return BettingAction::Call;

	//std::cout << "Action taken is FOLD\n";
	return BettingAction::Fold;
}

void Strategy::CalculateRequirements(unsigned int _PlayerAmt)
{
	float MeanExpectation = 1.0f / (float)_PlayerAmt;
	float StratifiedExpectation = (1.0f - MeanExpectation) / 3.0f;

	Requirements[0] = MeanExpectation + (Thresholds[0] * StratifiedExpectation);
	Requirements[1] = MeanExpectation + (Thresholds[1] * StratifiedExpectation);
	Requirements[2] = MeanExpectation + (Thresholds[2] * StratifiedExpectation);
	Requirements[3] = MeanExpectation + (Thresholds[3] * StratifiedExpectation);


	//Requirements[0] = Thresholds[0] * MeanExpectation; //Callable Win-Rate
	//Requirements[1] = Thresholds[1] * StratifiedExpectation;//MinRaise Win-Rate
	//Requirements[2] = Thresholds[2] * StratifiedExpectation;//Halfpot Win-rate
	//Requirements[3] = Thresholds[3] * StratifiedExpectation;//Fullpot win-rate

	/*for (unsigned int Index = 0; Index < 4; Index++)
	{
		Requirements[Index] = Thresholds[Index] * (_PlayerAmt / 8.0f); //(1.0f / _PlayerAmt);
	}*/
}