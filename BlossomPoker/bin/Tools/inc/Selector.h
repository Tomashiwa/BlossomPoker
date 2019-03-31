#pragma once

#include <vector>
#include <memory>
#include <chrono>
#include <random>

#include "../../AI/inc/BlossomPlayer.h"
#include "../inc/TrainingModel.h"

class Selector
{
public:
	Selector(Selection _Method);
	~Selector();

	void SetMethod(Selection _Method);
	std::shared_ptr<BlossomPlayer> SelectFrom(std::vector<std::shared_ptr<BlossomPlayer>>& _Population);
	void ErasePrecomputation();

private:
	Selection Method;

	//Truncation
	float TruncationRatio = 0.125f;

	//Tournament
	unsigned int TournamentSize = 2;

	//Roulette & Universal Stochastic Sampling
	std::vector<std::shared_ptr<BlossomPlayer>> SortedPopulation;
	std::vector<float> FitnessSlices;
	unsigned int SamplingIndex = 0;
	unsigned int SamplingInterval = 8;

	//Tools
	std::mt19937 MTGenerator;

	std::shared_ptr<BlossomPlayer> TruncationSelect(std::vector<std::shared_ptr<BlossomPlayer>>& _Population);
	std::shared_ptr<BlossomPlayer> TournamentSelect(std::vector<std::shared_ptr<BlossomPlayer>>& _Population);
	std::shared_ptr<BlossomPlayer> RouletteSelect(std::vector<std::shared_ptr<BlossomPlayer>>& _Population);
	std::shared_ptr<BlossomPlayer> UniversalSamplingSelect(std::vector<std::shared_ptr<BlossomPlayer>>& _Population);
};

