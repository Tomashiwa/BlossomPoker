#include "../inc/CallRaiser.h"
#include "../../Table/inc/Table.h"

CallRaiser::CallRaiser(const std::shared_ptr<Table>& _Table, unsigned int _Index) : Player(_Table, _Index)
{
	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

BettingAction CallRaiser::DetermineAction()
{
	std::vector<BettingAction> AvaliableActions;
	GetAvaliableActions(AvaliableActions);

	std::uniform_int_distribution<int> Distribution_Sizing(0, 2);

	if (std::find(AvaliableActions.begin(), AvaliableActions.end(), BettingAction::Bet) != AvaliableActions.end())
	{
		switch (Distribution_Sizing(MTGenerator))
		{
		case 0:
			RaiseBetAmt = ResidingTable->GetBigBlind();
			break;
		case 1:
			RaiseBetAmt = ResidingTable->GetPot() / 2;
			break;
		case 2:
			RaiseBetAmt = ResidingTable->GetPot();
			break;
		}

		return BettingAction::Bet;
	}
	else if (std::find(AvaliableActions.begin(), AvaliableActions.end(), BettingAction::Raise) != AvaliableActions.end())
	{
		switch (Distribution_Sizing(MTGenerator))
		{
		case 0:
			RaiseBetAmt = ResidingTable->GetPrevRaiseBet() == 0 ? ResidingTable->GetBigBlind() : ResidingTable->GetPrevRaiseBet();
			break;
		case 1:
			RaiseBetAmt = ResidingTable->GetPot() / 2;
			break;
		case 2:
			RaiseBetAmt = ResidingTable->GetPot();
			break;
		}

		return BettingAction::Raise;
	}
	else if (std::find(AvaliableActions.begin(), AvaliableActions.end(), BettingAction::Call) != AvaliableActions.end())
	{
		return BettingAction::Call;
	}
	else if (std::find(AvaliableActions.begin(), AvaliableActions.end(), BettingAction::Check) != AvaliableActions.end())
	{
		return BettingAction::Check;
	}

	return BettingAction::Fold;
}