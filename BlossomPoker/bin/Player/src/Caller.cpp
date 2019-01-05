#include "../inc/Caller.h"

Caller::Caller(const std::shared_ptr<Table>& _Table, unsigned int _Index) : Player(_Table, _Index)
{}

BettingAction Caller::DetermineAction()
{
	std::vector<BettingAction> AvaliableActions;
	GetAvaliableActions(AvaliableActions);

	if (std::find(AvaliableActions.begin(), AvaliableActions.end(), BettingAction::Call) != AvaliableActions.end())
		return BettingAction::Call;

	return BettingAction::Fold;
}

