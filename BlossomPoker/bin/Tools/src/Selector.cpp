#include "../inc/Selector.h"

Selector::Selector(Selection _Method) : Method(_Method)
{
	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

Selector::~Selector()
{}

void Selector::SetMethod(Selection _Method)
{
	Method = _Method;
}

std::shared_ptr<BlossomPlayer> Selector::SelectFrom(std::vector<std::shared_ptr<BlossomPlayer>>& _Population)
{
	if (Method == Selection::Truncation)
		return TruncationSelect(_Population);

	else if (Method == Selection::Tour)
		return TournamentSelect(_Population);

	else if (Method == Selection::Roulette)
		return RouletteSelect(_Population);

	else if (Method == Selection::StochasticSampling)
		return UniversalSamplingSelect(_Population);

	return _Population[0];
}

void Selector::ErasePrecomputation()
{
	SortedPopulation.clear();
	FitnessSlices.clear();
	SamplingIndex = 0;
}

std::shared_ptr<BlossomPlayer> Selector::TruncationSelect(std::vector<std::shared_ptr<BlossomPlayer>>& _Population)
{
	unsigned int TruncatedIndex = (unsigned int)(TruncationRatio * (float)_Population.size());
	std::uniform_int_distribution<unsigned int> Distribution_Truncation(0, TruncatedIndex);

	return _Population[Distribution_Truncation(MTGenerator)];
}

std::shared_ptr<BlossomPlayer> Selector::TournamentSelect(std::vector<std::shared_ptr<BlossomPlayer>>& _Population)
{
	std::uniform_int_distribution<unsigned int> Distribution_Tournament(0, _Population.size() - 1);
	std::vector<std::shared_ptr<BlossomPlayer>> Tournament;

	for (unsigned int Index = 0; Index < TournamentSize; Index++)
		Tournament.push_back(_Population[Distribution_Tournament(MTGenerator)]);

	//std::cout << "Performing Tournament Selection...\n";

	//std::cout << "Selected for Tournament comparison of size " << TournamentSize << ":\n";
	//for (auto const& Player : Tournament)
	//	std::cout << "P." << Player->GetIndex() << " - " << Player->GetFitness() << "\n";
	//std::cout << "\n";

	std::shared_ptr<BlossomPlayer> BestPlayer = Tournament[0];

	for (auto const& Player : Tournament)
	{
		if (Player->GetFitness() > BestPlayer->GetFitness())
			BestPlayer = Player;
	}
	
	//std::cout << "Best player in tournament is P." << BestPlayer->GetIndex() << "\n";

	return BestPlayer;
}

std::shared_ptr<BlossomPlayer> Selector::RouletteSelect(std::vector<std::shared_ptr<BlossomPlayer>>& _Population)
{
	if (SortedPopulation.size() == 0)
	{
		SortedPopulation.insert(SortedPopulation.end(), _Population.begin(), _Population.end());

		//Sort from lowest to highest fitness
		std::sort(SortedPopulation.begin(), SortedPopulation.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) { return _First->GetFitness() > _Second->GetFitness();  });

		float FitnessSum = 0.0f;
		for (auto const& Player : _Population)
			FitnessSum += Player->GetFitness();

		FitnessSlices.clear();
		for (auto const& Player : _Population)
			FitnessSlices.push_back(Player->GetFitness() / FitnessSum);
	}

	std::uniform_real_distribution<float> Distribution_Spin(0.0f, 1.0f);
	float SpinValue = Distribution_Spin(MTGenerator);

	for (unsigned int Index = FitnessSlices.size() - 1; Index >= 0; Index--)
	{
		if (Index != 0 && FitnessSlices[Index] >= SpinValue && FitnessSlices[Index - 1] <= SpinValue)
			return _Population[Index];
	}

	return _Population[_Population.size() - 1];
}

std::shared_ptr<BlossomPlayer> Selector::UniversalSamplingSelect(std::vector<std::shared_ptr<BlossomPlayer>>& _Population)
{
	if (SortedPopulation.size() == 0)
	{
		SortedPopulation.insert(SortedPopulation.end(), _Population.begin(), _Population.end());

		//Sort from lowest to highest fitness
		std::sort(SortedPopulation.begin(), SortedPopulation.end(), [&](std::shared_ptr<BlossomPlayer> _First, std::shared_ptr<BlossomPlayer> _Second) { return _First->GetFitness() > _Second->GetFitness(); });

		float FitnessSum = 0.0f;
		for (auto const& Player : _Population)
			FitnessSum += Player->GetFitness();

		FitnessSlices.clear();
		for (auto const& Player : _Population)
			FitnessSlices.push_back(Player->GetFitness() / FitnessSum);

		std::uniform_int_distribution<unsigned int> Distribution_Sampling(0, SortedPopulation.size() - 1);
		SamplingIndex = Distribution_Sampling(MTGenerator);
	}

	float SpinValue = (float)(SamplingIndex + SamplingInterval) / (float)_Population.size();
	SamplingIndex = (SamplingIndex + SamplingInterval) > SortedPopulation.size() ? ((SamplingIndex + SamplingInterval) - SortedPopulation.size()) : (SamplingIndex + SamplingInterval);

	for (unsigned int Index = FitnessSlices.size() - 1; Index >= 0; Index--)
	{
		if (Index != 0 && FitnessSlices[Index] >= SpinValue && FitnessSlices[Index - 1] <= SpinValue)
			return _Population[Index];
	}

	return _Population[_Population.size() - 1];
}