#include "Folder.h"

Folder::Folder(const std::shared_ptr<Table>& _Table, unsigned int _Index) : Player(_Table,_Index)
{}

BettingAction Folder::DetermineAction()
{
	std::vector<BettingAction> AvaliableActions;
	GetAvaliableActions(AvaliableActions);

	if (std::find(AvaliableActions.begin(), AvaliableActions.end(), BettingAction::Check) != AvaliableActions.end())
		return BettingAction::Check;

	return BettingAction::Fold;
}
