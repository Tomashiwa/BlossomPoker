#include "../inc/BlossomPlayer.h"
#include "../../Table/inc/Table.h"

BlossomPlayer::BlossomPlayer(const std::shared_ptr<Table>& _Table, const std::shared_ptr<HandEvaluator>& _Evaluator, unsigned int _Index) : Player(_Table,_Index)
{
	AI.Initialise();
	AI.SetEvalutor(_Evaluator);
}

BlossomPlayer::BlossomPlayer(const std::shared_ptr<BlossomPlayer>& _Reference, unsigned int _Index) : Player(_Reference->GetTable(), _Index)
{
	AI.Initialise();
	AI.SetEvalutor(_Reference->GetAI().GetEvaluator());
	AI.SetThresholds(_Reference->GetAI().GetThresholds());
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

float BlossomPlayer::CalculateUniqueness(const std::vector<std::shared_ptr<BlossomPlayer>>& _Population)
{
	auto PlayerItr = std::find_if(_Population.begin(), _Population.end(), [&](const std::shared_ptr<BlossomPlayer>& _Comparison) { return GetIndex() == _Comparison->GetIndex(); });

	if (PlayerItr == _Population.begin() || PlayerItr == _Population.end() - 1 || PlayerItr == _Population.end())
		return std::numeric_limits<float>::infinity();

	//Order is reversed as the Population is sorted in descending fitness
	std::shared_ptr<BlossomPlayer> Current = *PlayerItr;
	std::shared_ptr<BlossomPlayer> Inferior =(*(std::next(PlayerItr, 1)));
	std::shared_ptr<BlossomPlayer> Superior =(*(std::prev(PlayerItr, 1)));
	Current->SetUniqueness(Superior->GetFitness() - Inferior->GetFitness());

	return Current->GetUniqueness();
}

float BlossomPlayer::CalculatePotential(const std::vector<std::shared_ptr<BlossomPlayer>>& _Population, unsigned int _SamplingBreadth)
{
	auto PlayerItr = std::find_if(_Population.begin(), _Population.end(), [&](const std::shared_ptr<BlossomPlayer>& _Comparison) { return GetIndex() == _Comparison->GetIndex(); });

	if (PlayerItr == _Population.begin() || PlayerItr == _Population.end() - 1 || PlayerItr == _Population.end())
		return std::numeric_limits<float>::infinity();

	float AverageFitness = 0.0f;
	unsigned int NeighbourCount = 0;

	//Sampling towards superior
	auto SuperiorItr = PlayerItr;
	for (unsigned int Index = 0; Index < _SamplingBreadth; Index++)
	{
		if (SuperiorItr == _Population.begin())
			break;

		SuperiorItr = std::prev(SuperiorItr, 1);
		AverageFitness += (*(SuperiorItr))->GetFitness();
		NeighbourCount++;
	}

	//Sampling towards inferior
	auto InferiorItr = PlayerItr;
	if (std::next(InferiorItr, 1) != _Population.end())
	{
		for (unsigned int Index = 0; Index < _SamplingBreadth; Index++)
		{
			InferiorItr = std::next(InferiorItr, 1);
			AverageFitness += (*(InferiorItr))->GetFitness();
			NeighbourCount++;

			if (std::next(InferiorItr, 1) == _Population.end())
				break;
		}
	}

	AverageFitness /= NeighbourCount;
	SetPotential(AverageFitness - GetFitness());
	return GetPotential();
}

void BlossomPlayer::ClearStats()
{
	Fitness = 0.0f;
	Uniqueness = 0.0f;
	Potential = 0.0f;

	MoneyWon = 0;
	MoneyLost = 0;
	HandsWon = 0;
	HandsLost = 0;

	Profits = 0;
}