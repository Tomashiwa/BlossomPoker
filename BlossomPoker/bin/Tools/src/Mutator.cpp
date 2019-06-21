#include "../inc/Mutator.h"

Mutator::Mutator(Mutation _Method, float _Probability) : Method(_Method), Probability(_Probability)
{
	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

Mutator::~Mutator()
{}

void Mutator::SetMethod(Mutation _Method, float _Probability)
{
	Method = _Method;
	Probability = _Probability;
}

void Mutator::Mutate(std::shared_ptr<BlossomPlayer>& _Target)
{
	if (Method == Mutation::GaussianOffset)
		return GaussianOffset(_Target);
}

void Mutator::AdaptProbability()
{
	//Custom Diversity-based Adaptive Mutation
	/*float Sensitivity = 0.5f;//0.1f;
	float Diversity_Min = 0.75f, Diversity_Max = 1.3f;
	float Diversity_Current = GetGenerationDiversity();

	if (MutatePhase == 0)
	{
	Model.MutationRate = Model.MutationRate * (1 + (Sensitivity * ((Diversity_Min - Diversity_Current) / Diversity_Current)));
	MutatePhase = Diversity_Current <= Diversity_Min ? 1 : 0;
	}
	else
	{
	Model.MutationRate = Model.MutationRate * (1 + (Sensitivity * ((Diversity_Max - Diversity_Current) / Diversity_Current)));
	MutatePhase = Diversity_Current >= Diversity_Max ? 0 : 1;
	}

	Model.MutationRate = std::max(0.0f, std::min(Model.MutationRate, 0.5f));*/

	//Diversity-based Adaptive Mutation
	//float Sensitivity = 0.225f;//0.3f;
	//float TargetDiversity = 0.8f;
	//float CurrentDiversity = GetGenerationDiversity();

	//Model.MutationRate = Model.MutationRate * (1 + (Sensitivity * ((TargetDiversity - CurrentDiversity) / CurrentDiversity)));
	//Model.MutationRate = std::max(0.0f, std::min(Model.MutationRate, 0.5f));

	//Gaussian Distribution
	//float a = 2.5f, b = 0.5f, c = 1.5075f;//float a = 2.5f, b = 0.5f, c = 0.15f;
	//float x = (float)Generation / (float)Model.GenerationLimit;
	//float e = std::exp(1.0f);
	//Model.MutationRate = pow((a * e), -(pow((x - b), 2) / (2 * pow(c, 2)))) - 0.9f;//pow((a * e), -(pow((x - b), 2) / (2 * pow(c, 2))));

	//Oscillating Sine Wave
	/*float Freq = 48.7f;
	float HeightOffset = 0.5f;
	float GenRatio = (float)Generation / (float)Model.GenerationLimit;
	Model.MutationRate = (sin(Freq * sqrt(GenRatio))) / 2.0f + HeightOffset;

	Model.MutationRate = std::max(0.0f, std::min(Model.MutationRate, 1.0f));*/

	//Writer->WriteAt(3, (float)Generation, Model.MutationRate);
}

void Mutator::GaussianOffset(std::shared_ptr<BlossomPlayer>& _Target)
{
	std::array<float, 16> Thresholds = _Target->GetAI().GetThresholds();
	
	std::uniform_real_distribution<float> Distribution_Probability(0.0f, 1.0f);
	std::uniform_real_distribution<float> Distribution_Offset(-OffsetAmt, OffsetAmt);

	for (auto& Threshold : Thresholds)
	{
		if (Distribution_Probability(MTGenerator) <= Probability)
			Threshold += Distribution_Offset(MTGenerator);
	}

	_Target->GetAI().SetThresholds(Thresholds);
}

std::string Mutator::GetMethodStr(Mutation _Method)
{
	std::string MethodName;

	if (_Method == Mutation::GaussianOffset)
		MethodName = "Gaussian Offset from -" + std::to_string(OffsetAmt) + " to " + std::to_string(OffsetAmt) + " - " + std::to_string(Probability * 100.0f) + "%";

	else
		MethodName = "UNDEFINED";

	return MethodName;
}
