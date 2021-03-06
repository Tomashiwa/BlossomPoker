#include "../inc/Randomer.h"
#include "../../Table/inc/Table.h"

Randomer::Randomer(const std::shared_ptr<Table>& _Table, unsigned int _Index) : Player(_Table,_Index)
{
	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

BettingAction Randomer::DetermineAction()
{
	std::vector<BettingAction> AvaliableActions;
	GetAvaliableActions(AvaliableActions);

	std::uniform_int_distribution<int> Distribution_Action(0, AvaliableActions.size() - 1);
	std::uniform_int_distribution<int> Distribution_Sizing(0, 2);

	switch (AvaliableActions[Distribution_Action(MTGenerator)])
	{
	case BettingAction::Bet:
		RaiseBetAmt = 0;

		switch (Distribution_Sizing(MTGenerator))
		{
		case 0:
			RaiseBetAmt = ResidingTable->GetBigBlind();//RaiseBetAmt += ResidingTable->GetBigBlind();
			break;
		case 1:
			RaiseBetAmt = ResidingTable->GetPot() / 2;//RaiseBetAmt += ResidingTable->GetPot() / 2;
			break;
		case 2:
			RaiseBetAmt = ResidingTable->GetPot();//RaiseBetAmt += ResidingTable->GetPot();
			break;
		}

		return BettingAction::Bet;
		break;

	case BettingAction::Raise:
		switch (Distribution_Sizing(MTGenerator))
		{
		case 0:
			RaiseBetAmt = ResidingTable->GetPrevRaiseBet() == 0 ? ResidingTable->GetBigBlind() : ResidingTable->GetPrevRaiseBet();//RaiseBetAmt += ResidingTable->GetPrevRaiseBet() == 0 ? ResidingTable->GetBigBlind() : ResidingTable->GetPrevRaiseBet();
			break;
		case 1:
			RaiseBetAmt = ResidingTable->GetPot() / 2;//RaiseBetAmt += ResidingTable->GetPot() / 2;
			break;
		case 2:
			RaiseBetAmt = ResidingTable->GetPot();//RaiseBetAmt += ResidingTable->GetPot();
			break;
		}

		return BettingAction::Raise;
		break;

	case BettingAction::Call:
		return BettingAction::Call;
		break;

	case BettingAction::Check:
		return BettingAction::Check;
		break;
	}

	return BettingAction::Fold;
}

