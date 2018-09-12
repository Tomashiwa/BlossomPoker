#include "Orchastrator.h"
#include "BlossomAI.h"
#include "Strategy.h"

Orchastrator::Orchastrator(std::shared_ptr<BlossomAI> _AI)
{
	AI = _AI;
}

Orchastrator::~Orchastrator()
{
}

void Orchastrator::InitializeStrat(std::array<double, 8> _Thresholds)
{
	std::shared_ptr<Orchastrator> ThisShared = std::move(shared_from_this());

	Strategies[0] = std::make_shared<Strategy>(ThisShared, _Thresholds[0], _Thresholds[1]);
	Strategies[1] = std::make_shared<Strategy>(ThisShared, _Thresholds[2], _Thresholds[3]);
	Strategies[2] = std::make_shared<Strategy>(ThisShared, _Thresholds[4], _Thresholds[5]);
	Strategies[3] = std::make_shared<Strategy>(ThisShared, _Thresholds[6], _Thresholds[7]);

	CurrentStrategy = nullptr;
}

void Orchastrator::InitializeRandomStrat()
{
	//Use Mersenne Twister to obtain a random card to insert into Set
	auto Seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

	std::mt19937 mt(Seed);
	std::uniform_real_distribution<double> ThresholdDistribution(0.0, 1.0);

	std::shared_ptr<Orchastrator> ThisShared = std::move(shared_from_this());

	Strategies[0] = std::make_shared<Strategy>(ThisShared, ThresholdDistribution(mt), ThresholdDistribution(mt));
	Strategies[1] = std::make_shared<Strategy>(ThisShared, ThresholdDistribution(mt), ThresholdDistribution(mt));
	Strategies[2] = std::make_shared<Strategy>(ThisShared, ThresholdDistribution(mt), ThresholdDistribution(mt));
	Strategies[3] = std::make_shared<Strategy>(ThisShared, ThresholdDistribution(mt), ThresholdDistribution(mt));

	CurrentStrategy = nullptr;
}

void Orchastrator::SetOptimalStrategy(std::shared_ptr<Strategy>& _Strategy)
{
	switch (AI->GetSnapshot().Phase)
	{
		case Phase::Preflop:
			_Strategy = Strategies[0];

		case Phase::Flop:
			_Strategy = Strategies[1];

		case Phase::River:
			_Strategy = Strategies[2];

		case Phase::Turn:
			_Strategy = Strategies[3];
	}
}

BettingAction Orchastrator::DetermineAction()
{
	SetOptimalStrategy(CurrentStrategy);
	return CurrentStrategy->DetermineIdealAction();
}

void Orchastrator::SetThreshold(unsigned int _Index, double _Value)
{
	switch (_Index)
	{
		case 0:
			Strategies[0]->SetThreshold(0, _Value);
		case 1:
			Strategies[0]->SetThreshold(1, _Value);
		case 2:
			Strategies[1]->SetThreshold(0, _Value);
		case 3:
			Strategies[1]->SetThreshold(1, _Value);
		case 4:
			Strategies[2]->SetThreshold(0, _Value);
		case 5:
			Strategies[2]->SetThreshold(1, _Value);
		case 6:
			Strategies[3]->SetThreshold(0, _Value);
		case 7:
			Strategies[3]->SetThreshold(1, _Value);
		default:
			break;
	}
}

std::array<double, 8> Orchastrator::GetThresholds()
{
	std::array<double, 8> Thresholds;
	Thresholds[0] = Strategies[0]->GetThresholds()[0];
	Thresholds[1] = Strategies[0]->GetThresholds()[1];

	Thresholds[2] = Strategies[1]->GetThresholds()[0];
	Thresholds[3] = Strategies[1]->GetThresholds()[1];

	Thresholds[4] = Strategies[2]->GetThresholds()[0];
	Thresholds[5] = Strategies[2]->GetThresholds()[1];

	Thresholds[6] = Strategies[3]->GetThresholds()[0];
	Thresholds[7] = Strategies[3]->GetThresholds()[1];
	return Thresholds;
}

