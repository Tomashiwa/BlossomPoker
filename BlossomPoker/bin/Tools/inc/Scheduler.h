#pragma once
#include <array>
#include <vector>
#include <memory>

#include "GeneticTrainer.h"
#include "TrainingModel.h"

#include <windows.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")

class Scheduler
{
public:
	Scheduler();
	~Scheduler();

	void Initialize();
	void Run(bool _TerminateAfterComp);
	void End();

	BOOL Terminate();

	void Add(TrainingModel _Model, Layer _Layer);

private:
	std::unique_ptr<GeneticTrainer> Trainer;
	std::vector<std::pair<TrainingModel,Layer>> Specs;
};

