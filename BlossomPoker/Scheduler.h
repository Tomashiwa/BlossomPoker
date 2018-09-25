#pragma once
#include <array>
#include <vector>
#include <memory>

#include "GeneticTest.h"

class Scheduler
{
public:
	Scheduler();
	~Scheduler();

	void Run();

	void Start();
	void End();

	void Add(unsigned int _PopulationSize, unsigned int _SubjectsSize, unsigned int _GenerationLimit);

private:
	struct Specification
	{
		unsigned int PopulationSize;
		unsigned int SubjectsAmt;
		unsigned int GenerationLimit;

		Specification(unsigned int _PopulationSize, unsigned int _SubjectsSize, unsigned int _GenerationLimit) 
			: PopulationSize(_PopulationSize), SubjectsAmt(_SubjectsSize), GenerationLimit(_GenerationLimit) {}
	};

	std::vector<std::unique_ptr<Specification>> Specs;
	std::unique_ptr<GeneticTest> Test;
};

