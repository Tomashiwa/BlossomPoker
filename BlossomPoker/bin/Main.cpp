#include <iostream>
#include <string>
#include <memory>

#include "../bin/Tools/inc/GeneticTrainer.h"
#include "../bin/Tools/inc/Scheduler.h"

int main()
{
	std::unique_ptr<Scheduler> Schedule = std::make_unique<Scheduler>();

	TrainingModel Model;
	Model.PopulationSize = 500;
	Model.GenerationLimit = 500;
	Model.TournamentsPerGen = 9;

	Model.HasHoF = true;

	Model.SelectMethod = Selection::Tour;
	Model.TournamentSize = 2;

	Model.CrossMethod = Crossover::KPoint;
	Model.KPointCount = 3;
	Model.CrossoverRate = 0.5f;

	Model.MutateMethod = Mutation::GaussianOffset;
	Model.GaussianOffset = 0.25f;
	Model.MutationRate = 0.01f;

	Layer FeedbackLayer = Layer::Generational;

	Schedule->Add(Model, FeedbackLayer);

	Schedule->Run(false);
	system("pause");

	return 0;
}