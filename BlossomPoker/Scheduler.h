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

	void Start();
	void Run();
	void End();

	void Add(unsigned int _PopulationSize, unsigned int _GenerationLimit);

private:
	struct Specification
	{
		unsigned int PopulationSize;
		unsigned int GenerationLimit;

		Specification(unsigned int _PopulationSize, unsigned int _GenerationLimit) 
			: PopulationSize(_PopulationSize), GenerationLimit(_GenerationLimit) {}
	};

	std::vector<std::unique_ptr<Specification>> Specs;
	std::unique_ptr<GeneticTest> Test;
};

