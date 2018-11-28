#include "Strategy.h"
#include "BlossomAI.h"

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

	CalculateRequirements(_PlayerAmt);
	//std::cout << "Win Rate: " << _WinRate << " / ToCall: " << Requirements[0] << ", ToHalfPot: " << Requirements[1] << ", ToFullPot: " << Requirements[2] << ", ToAllIn: " << Requirements[3] << "\n";

	if (_WinRate >= Requirements[3])
		Sizing = RaiseBetSize::AllIn;
	else if (_WinRate >= Requirements[2])
		Sizing = RaiseBetSize::Pot;
	else if (_WinRate >= Requirements[1])
		Sizing = RaiseBetSize::HalfPot;
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
	for (unsigned int Index = 0; Index < 4; Index++)
		Requirements[Index] = Thresholds[Index] * (1.0 / _PlayerAmt);
}