#pragma once

#include <vector>
#include <memory>
#include <chrono>
#include <random>

#include "../../AI/inc/BlossomPlayer.h"
#include "../inc/TrainingModel.h"

class Crossoverer
{
public:
	Crossoverer(Crossover _Method, float _Probability);
	~Crossoverer();
	
	void SetMethod(Crossover _Method, float _Probability);
	void Cross(std::array<std::shared_ptr<BlossomPlayer>, 2>& _Parents, std::array<std::shared_ptr<BlossomPlayer>, 2>& _Children);

	void SetPointsAmt(unsigned int _Amt) { PointsAmt = _Amt; }
	float GetProbability() { return Probability; }
	std::string GetMethodStr(Crossover _Method);

private:
	Crossover Method;
	float Probability = 0.5f;

	//K-Point Crossover
	unsigned int PointsAmt = 2;

	//Tools
	std::mt19937 MTGenerator;

	void KPointCross(std::array<std::shared_ptr<BlossomPlayer>, 2>& _Parents, std::array<std::shared_ptr<BlossomPlayer>, 2>& _Children);
	void UniformCross(std::array<std::shared_ptr<BlossomPlayer>, 2>& _Parents, std::array<std::shared_ptr<BlossomPlayer>, 2>& _Children);
	void BlendingCross(std::array<std::shared_ptr<BlossomPlayer>, 2>& _Parents, std::array<std::shared_ptr<BlossomPlayer>, 2>& _Children);
};

