#pragma once
#include <array>
#include <vector>
#include <memory>

#include "GeneticTrainer.h"

#include <windows.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")

class Scheduler
{
public:
	Scheduler();
	~Scheduler();

	void Start();
	void Run(bool _TerminateAfterComp);
	void End();

	BOOL Terminate();

	void Add(unsigned int _PopulationSize, unsigned int _GenerationLimit, unsigned int _ToursPerGen);

private:
	struct Specification
	{
		unsigned int PopulationSize;
		unsigned int GenerationLimit;
		unsigned int ToursPerGen;

		Specification(unsigned int _PopulationSize, unsigned int _GenerationLimit, unsigned int _ToursPerGen)
			: PopulationSize(_PopulationSize), GenerationLimit(_GenerationLimit), ToursPerGen(_ToursPerGen) {}
	};

	std::vector<std::unique_ptr<Specification>> Specs;
	std::unique_ptr<GeneticTrainer> Test;
};

