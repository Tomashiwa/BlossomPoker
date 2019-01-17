#include "../inc/BlossomPlayer.h"
#include "../../Table/inc/Table.h"

BlossomPlayer::BlossomPlayer(const std::shared_ptr<Table>& _Table, const std::shared_ptr<HandEvaluator>& _Evaluator, unsigned int _Index) : Player(_Table,_Index)
{
	AI.Initialise();
	AI.SetEvalutor(_Evaluator);
}

BettingAction BlossomPlayer::DetermineAction()
{
	Snapshot NewShot;
	NewShot.Stack = GetStack();
	NewShot.RequiredAnte = ResidingTable->GetRequiredAnte();
	NewShot.Pot = ResidingTable->GetPot();

	std::vector<std::shared_ptr<Player>> ActivePlayers;
	ResidingTable->GetActivePlayers(ActivePlayers);
	NewShot.PlayerAmt = ActivePlayers.size();
	
	NewShot.BB = ResidingTable->GetBigBlind();
	NewShot.Rounds = ResidingTable->GetRounds();
	NewShot.Phase = ResidingTable->GetState();
	NewShot.Hole = Hand;
	NewShot.CurrentAnte = GetAnte();
	NewShot.Contribution = GetPotContribution();
	NewShot.Communal = ResidingTable->GetCommunalCards();
	NewShot.PrevRaiseBet = ResidingTable->GetPrevRaiseBet();

	GetAvaliableActions(NewShot.AvaliableActions);

	BettingAction IdealAction = AI.EnquireAction(NewShot);
	RaiseBetAmt = AI.GetRaiseBetAmt();

	return IdealAction;
}