#pragma once
#include "../../Player/inc/Player.h"
#include "BlossomAI.h"

class BlossomPlayer : public Player
{
public:
	BlossomPlayer(const std::shared_ptr<Table>& _Table, const std::shared_ptr<HandEvaluator>& _Evaluator, unsigned int _Index);

	BettingAction DetermineAction();

	float CalculateUniqueness(const std::vector<std::shared_ptr<BlossomPlayer>>& _Population);
	float CalculatePotential(const std::vector<std::shared_ptr<BlossomPlayer>>& _Population, unsigned int _SamplingBreadth);

	void ClearStats();

	//Setters & Getters
	BlossomAI& GetAI() { return AI; }

	void SetUniqueness(float _Uniqueness) { Uniqueness = _Uniqueness; }
	void SetPotential(float _Potential) { Potential = _Potential; }

	float GetUniqueness() { return Uniqueness; }
	float GetPotential() { return Potential; }

private:
	BlossomAI AI;

	float Uniqueness = 0.0f;
	float Potential = 0.0f;
};

