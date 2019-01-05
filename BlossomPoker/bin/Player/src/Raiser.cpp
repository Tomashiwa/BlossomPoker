#include "Raiser.h"
#include "Table.h"

Raiser::Raiser(const std::shared_ptr<Table>& _Table, unsigned int _Index) : Player(_Table, _Index)
{
	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

BettingAction Raiser::DetermineAction()
{
	std::vector<BettingAction> AvaliableActions;
	GetAvaliableActions(AvaliableActions);

	std::uniform_int_distribution<int> Distribution_Sizing(0, 2);

	if (std::find(AvaliableActions.begin(), AvaliableActions.end(), BettingAction::Bet) != AvaliableActions.end())
	{
		//RaiseBetAmt = 0;

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
	}

	else if (std::find(AvaliableActions.begin(), AvaliableActions.end(), BettingAction::Raise) != AvaliableActions.end())
	{
		//RaiseBetAmt = ResidingTable->GetRequiredAnte() - GetAnte();

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
	}

	return BettingAction::Fold;
}
