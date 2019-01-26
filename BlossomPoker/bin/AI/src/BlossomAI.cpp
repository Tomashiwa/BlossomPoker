#include "../inc/BlossomAI.h"

BlossomAI::BlossomAI()
{
}

BlossomAI::~BlossomAI()
{
}

void BlossomAI::Initialise()
{
	MT.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<float> ThresholdDistribution(0.0, 2.0);

	for (unsigned int StratIndex = 0; StratIndex < 4; StratIndex++)
	{
		Strategies[StratIndex] = std::make_shared<Strategy>((Phase)StratIndex);

		std::array<float, 4> RandomThresholds;
		for (unsigned int ThrIndex = 0; ThrIndex < 4; ThrIndex++)
			RandomThresholds[ThrIndex] = ThresholdDistribution(MT);

		std::sort(RandomThresholds.begin(), RandomThresholds.end(),
			[](float _First, float _Second)
			{return _First < _Second; });

		for(unsigned int ThrIndex = 0; ThrIndex < 4; ThrIndex++)
			Strategies[StratIndex]->SetThreshold(ThrIndex, RandomThresholds[ThrIndex]);

		Thresholds[0 + (4 * StratIndex)] = Strategies[StratIndex]->GetThresholds()[0];
		Thresholds[1 + (4 * StratIndex)] = Strategies[StratIndex]->GetThresholds()[1];	
		Thresholds[2 + (4 * StratIndex)] = Strategies[StratIndex]->GetThresholds()[2];
		Thresholds[3 + (4 * StratIndex)] = Strategies[StratIndex]->GetThresholds()[3];
	}

	//std::cout << "Strat 1: " << Strategies[0]->GetThresholds()[0] << "," << Strategies[0]->GetThresholds()[1] << "," << Strategies[0]->GetThresholds()[2] << "," << Strategies[0]->GetThresholds()[3] << "\n";
	//std::cout << "Strat 2: " << Strategies[1]->GetThresholds()[0] << "," << Strategies[1]->GetThresholds()[1] << "," << Strategies[1]->GetThresholds()[2] << "," << Strategies[1]->GetThresholds()[3] << "\n";
	//std::cout << "Strat 3: " << Strategies[2]->GetThresholds()[0] << "," << Strategies[2]->GetThresholds()[1] << "," << Strategies[2]->GetThresholds()[2] << "," << Strategies[2]->GetThresholds()[3] << "\n";
	//std::cout << "Strat 4: " << Strategies[3]->GetThresholds()[0] << "," << Strategies[3]->GetThresholds()[1] << "," << Strategies[3]->GetThresholds()[2] << "," << Strategies[3]->GetThresholds()[3] << "\n";
	//std::cout << "\n";
}

void BlossomAI::Reset()
{
}

BettingAction BlossomAI::EnquireAction(Snapshot _Snapshot)
{
	//std::cout << "Determining Ideal Action...\n";
	if (CurrentRound != _Snapshot.Rounds)
	{
		CurrentRound = _Snapshot.Rounds;
		CurrentPhase = Phase::NONE;
	}

	if (CurrentPhase == Phase::NONE || CurrentPhase != _Snapshot.Phase)
	{
		CurrentPhase = _Snapshot.Phase;
		ActiveStrategy = GetStrategy(CurrentPhase);
		CurrentWinRate = DetermineWinRate(_Snapshot.Hole, _Snapshot.Communal, _Snapshot.PlayerAmt - 1) / 100.0f;
	}

	//ActiveStrategy = GetStrategy(_Snapshot.Phase);
	//BettingAction IdealAction = ActiveStrategy->DetermineAction(_Snapshot.AvaliableActions, DetermineWinRate(_Snapshot.Hole, _Snapshot.Communal, _Snapshot.PlayerAmt - 1) / 100.0f, _Snapshot.PlayerAmt); 
	BettingAction IdealAction = ActiveStrategy->DetermineAction(_Snapshot.AvaliableActions, CurrentWinRate, _Snapshot.PlayerAmt);

	if (IdealAction == BettingAction::Bet)
	{
		switch (ActiveStrategy->GetSizing())
		{
			case RaiseBetSize::Minimum:
			{
				RaiseBetAmt = _Snapshot.BB;
				break;
			}
			case RaiseBetSize::HalfPot:
			{
				RaiseBetAmt = _Snapshot.Pot / 2;
				break;
			}
			case RaiseBetSize::Pot:
			{
				RaiseBetAmt = _Snapshot.Pot;
				break;
			}
		}
	}
	else if (IdealAction == BettingAction::Raise)
	{
		switch (ActiveStrategy->GetSizing())
		{
		case RaiseBetSize::Minimum:
		{
			RaiseBetAmt = _Snapshot.PrevRaiseBet == 0 ? _Snapshot.BB : _Snapshot.PrevRaiseBet;
			break;
		}
		case RaiseBetSize::HalfPot:
		{
			RaiseBetAmt = _Snapshot.Pot / 2;
			break;
		}
		case RaiseBetSize::Pot:
		{
			RaiseBetAmt = _Snapshot.Pot;
			break;
		}
		}
	}
	return IdealAction;
}

float BlossomAI::DetermineWinRate(std::array<Card, 2> _Hole, std::vector<Card> _Communal, unsigned int _OppoAmt)
{
	//unsigned int RunsLimit = 10;
	//float Current = 0.0f;
	//float Average = 0.0f;

	//std::cout << "Win-rate w/ 7500 iterations: \n";
	//for (unsigned int Index = 0; Index < RunsLimit; Index++)
	//{
	//	Current = Evaluator->DetermineOdds_MonteCarlo_Multi_OMPEval(_Hole, _Communal, _OppoAmt, 7500);
	//	Average += Current;

	//	//std::cout << Current << ", ";
	//}
	//Average /= RunsLimit;
	//std::cout << "Average: " << Average << "\n";
	//std::cout << "\n\n";

	//Average = 0.0f;

	//std::cout << "Win-rate w/ 5000 iterations: \n";
	//for (unsigned int Index = 0; Index < RunsLimit; Index++)
	//{
	//	Current = Evaluator->DetermineOdds_MonteCarlo_Multi_OMPEval(_Hole, _Communal, _OppoAmt, 5000);
	//	Average += Current;

	//	//std::cout << Current << ", ";
	//}
	//Average /= RunsLimit;
	//std::cout << "Average: " << Average << "\n";
	//std::cout << "\n\n";

	//Average = 0.0f;

	//std::cout << "Win-rate w/ 2500 iterations: \n";
	//for (unsigned int Index = 0; Index < RunsLimit; Index++)
	//{
	//	Current = Evaluator->DetermineOdds_MonteCarlo_Multi_OMPEval(_Hole, _Communal, _OppoAmt, 2500);
	//	Average += Current;

	//	//std::cout << Current << ", ";
	//}
	//Average /= RunsLimit;
	//std::cout << "Average: " << Average << "\n";
	//std::cout << "\n\n";

	//Average = 0.0f;

	//std::cout << "Win-rate w/ 1000 iterations: \n";
	//for (unsigned int Index = 0; Index < RunsLimit; Index++)
	//{
	//	Current = Evaluator->DetermineOdds_MonteCarlo_Multi_OMPEval(_Hole, _Communal, _OppoAmt, 1000);
	//	Average += Current;

	//	//std::cout << Current << ", ";
	//}
	//Average /= RunsLimit;
	//std::cout << "Average: " << Average << "\n";
	//std::cout << "\n";

	return Evaluator->DetermineOdds_MonteCarlo_Multi_OMPEval(_Hole, _Communal, _OppoAmt, 1000);
}

void BlossomAI::SetThresholds(std::array<float, 16> _Thresholds)
{
	for (unsigned int Index = 0; Index < 4; Index++)
	{
		SetThresholdByPhase((Phase) Index, 0, _Thresholds[0 + (Index * 4)]);
		SetThresholdByPhase((Phase) Index, 1, _Thresholds[1 + (Index * 4)]);
		SetThresholdByPhase((Phase) Index, 2, _Thresholds[2 + (Index * 4)]);
		SetThresholdByPhase((Phase)Index, 3, _Thresholds[3 + (Index * 4)]);
	}
}

void BlossomAI::SetThresholdByPhase(Phase _Phase, unsigned int _Index, float _Threshold)
{
	Thresholds[_Index + ((int)_Phase * 4)] = _Threshold;
	GetStrategy(_Phase)->SetThreshold(_Index, _Threshold);
}

void BlossomAI::SetThresholdsByPhase(Phase _Phase, std::array<float, 4> _Thresholds)
{
	Thresholds[0 + ((int)_Phase * 4)] = _Thresholds[0];
	Thresholds[1 + ((int)_Phase * 4)] = _Thresholds[1];
	Thresholds[2 + ((int)_Phase * 4)] = _Thresholds[2];
	Thresholds[3 + ((int)_Phase * 4)] = _Thresholds[3];

	GetStrategy(_Phase)->SetThresholds(_Thresholds);
}

std::array<float, 4> BlossomAI::GetThresholdsByPhase(Phase _Phase)  
{
	return GetStrategy(_Phase)->GetThresholds();
}

std::shared_ptr<Strategy> BlossomAI::GetStrategy(Phase _Phase)
{
	for (auto const& Strategy : Strategies)
	{
		if (Strategy->GetPhase() == _Phase)
			return Strategy;
	}

	return nullptr;
}
