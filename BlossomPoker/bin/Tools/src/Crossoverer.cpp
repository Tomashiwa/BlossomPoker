#include "..\inc\Crossoverer.h"

Crossoverer::Crossoverer(Crossover _Method, float _Probability) : Method(_Method), Probability(_Probability)
{
	MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

Crossoverer::~Crossoverer()
{}

void Crossoverer::SetMethod(Crossover _Method, float _Probability)
{
	Method = _Method;
	Probability = _Probability;
}

void Crossoverer::Cross(std::array<std::shared_ptr<BlossomPlayer>,2>& _Parents, std::array<std::shared_ptr<BlossomPlayer>,2>& _Children)
{
	std::uniform_real_distribution<float> Distribution_Probability(0.0f, 1.0f);

	if (Distribution_Probability(MTGenerator) > Probability)
	{
		_Children[0]->GetAI().SetThresholds(_Parents[0]->GetAI().GetThresholds());
		_Children[1]->GetAI().SetThresholds(_Parents[1]->GetAI().GetThresholds());
		return;
	}

	if (Method == Crossover::KPoint)
		KPointCross(_Parents, _Children);

	else if (Method == Crossover::Uniform)
		UniformCross(_Parents, _Children);

	else if (Method == Crossover::Blending)
		BlendingCross(_Parents, _Children);
}

void Crossoverer::KPointCross(std::array<std::shared_ptr<BlossomPlayer>, 2>& _Parents, std::array<std::shared_ptr<BlossomPlayer>, 2>& _Children)
{
	std::array<float, 16> Thresholds_Parents_0 = _Parents[0]->GetAI().GetThresholds();
	std::array<float, 16> Thresholds_Parents_1 = _Parents[1]->GetAI().GetThresholds();

	std::array<float, 16> Thresholds_Child_0, Thresholds_Child_1;

	std::vector<unsigned int> CrossIndexes;

	//Add two boundary points to facilitate comparision of crossing points
	CrossIndexes.push_back(0);
	CrossIndexes.push_back(Thresholds_Parents_0.size() - 1);

	//Add K randomly-generated crossing points
	std::uniform_int_distribution<unsigned int> Distribution_Index(1, Thresholds_Parents_0.size() - 2);
	for (unsigned int Count = 0; Count < PointsAmt; Count++)
	{
		unsigned int RandIndex = Distribution_Index(MTGenerator);

		while (std::find(CrossIndexes.begin(), CrossIndexes.end(), RandIndex) != CrossIndexes.end())
			RandIndex = Distribution_Index(MTGenerator);

		CrossIndexes.push_back(RandIndex);
	}

	std::sort(CrossIndexes.begin(), CrossIndexes.end(), [](unsigned int _First, unsigned int _Second) { return _First < _Second; });

	//Cross thresholds between parents (Only every odd segment will be crossed)
	unsigned int SegmentCount = 0;
	for (unsigned int Index = 1; Index < CrossIndexes.size(); Index++)
	{
		if (SegmentCount % 2 == 0)
		{
			for (unsigned int CopyAt = CrossIndexes[Index - 1]; CopyAt <= CrossIndexes[Index]; CopyAt++)
			{
				Thresholds_Child_0[CopyAt] = Thresholds_Parents_0[CopyAt];
				Thresholds_Child_1[CopyAt] = Thresholds_Parents_1[CopyAt];
			}

			SegmentCount++;
			continue;
		}

		for (unsigned int CrossAt = CrossIndexes[Index - 1]; CrossAt <= CrossIndexes[Index]; CrossAt++)
		{
			Thresholds_Child_0[CrossAt] = Thresholds_Parents_1[CrossAt];
			Thresholds_Child_1[CrossAt] = Thresholds_Parents_0[CrossAt];
		}

		SegmentCount++;
	}

	_Children[0]->GetAI().SetThresholds(Thresholds_Child_0);
	_Children[1]->GetAI().SetThresholds(Thresholds_Child_1);
}

void Crossoverer::UniformCross(std::array<std::shared_ptr<BlossomPlayer>, 2>& _Parents, std::array<std::shared_ptr<BlossomPlayer>, 2>& _Children)
{
	std::array<float, 16> Thresholds_Parent_0 = _Parents[0]->GetAI().GetThresholds();
	std::array<float, 16> Thresholds_Parent_1 = _Parents[1]->GetAI().GetThresholds();

	std::array<float, 16> Thresholds_Child_0, Thresholds_Child_1;

	std::uniform_int_distribution<unsigned int> Distribution_CrossFrom(0, 1);

	for (unsigned int Index = 0; Index < 16; Index++)
	{
		if (Distribution_CrossFrom(MTGenerator) == 0)
		{
			Thresholds_Child_0[Index] = Thresholds_Parent_0[Index];
			Thresholds_Child_1[Index] = Thresholds_Parent_1[Index];
		}
		else
		{
			Thresholds_Child_0[Index] = Thresholds_Parent_1[Index];
			Thresholds_Child_1[Index] = Thresholds_Parent_0[Index];
		}
	}

	_Children[0]->GetAI().SetThresholds(Thresholds_Child_0);
	_Children[1]->GetAI().SetThresholds(Thresholds_Child_1);
}

void Crossoverer::BlendingCross(std::array<std::shared_ptr<BlossomPlayer>, 2>& _Parents, std::array<std::shared_ptr<BlossomPlayer>, 2>& _Children)
{
	std::array<float, 16> Thresholds_Parent_0 = _Parents[0]->GetAI().GetThresholds();
	std::array<float, 16> Thresholds_Parent_1 = _Parents[1]->GetAI().GetThresholds();

	std::array<float, 16> Thresholds_Child_0, Thresholds_Child_1;

	for (unsigned int Index = 0; Index < 16; Index++)
	{
		std::uniform_real_distribution<float> Distribution_Blending(Thresholds_Parent_0[Index], Thresholds_Parent_1[Index]);

		Thresholds_Child_0[Index] = Distribution_Blending(MTGenerator);
		Thresholds_Child_1[Index] = Distribution_Blending(MTGenerator);
	}

	_Children[0]->GetAI().SetThresholds(Thresholds_Child_0);
	_Children[1]->GetAI().SetThresholds(Thresholds_Child_1);
}
