#pragma once

#include <vector>
#include <memory>
#include <chrono>
#include <random>

#include "../../AI/inc/BlossomPlayer.h"
#include "../inc/TrainingModel.h"

class Mutator
{
public:
	Mutator(Mutation _Method, float _Probability);
	~Mutator();

	void SetMethod(Mutation _Method, float _Probability);
	void Mutate(std::shared_ptr<BlossomPlayer>& _Target);
	void AdaptProbability();

	float GetProbability() { return Probability; }

private:
	Mutation Method;
	float Probability = 0.0f;

	//Gaussian Offset
	float OffsetAmt = 0.25f;

	//Tools
	std::mt19937 MTGenerator;
	
	void GaussianOffset(std::shared_ptr<BlossomPlayer>& _Target);
};

