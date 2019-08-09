#include <iostream>
#include <string>
#include <memory>

#include "GameManager.h"

#include "../bin/Tools/inc/GeneticTrainer.h"
#include "../bin/Tools/inc/Scheduler.h"

int main()
{
	OOPoker::GameManager Manager;
	Manager.DoGame();

	/*std::unique_ptr<Scheduler> Schedule = std::make_unique<Scheduler>();

	TrainingModel Model;
	
	Model.FeedbackLayer = Layer::Generational;

	Model.IsOverlapping = false; 
	Model.ChildPopulationRatio = 1.0f;

	Model.PopulationSize = 500; 
	Model.GenerationLimit = 1000;
	Model.TournamentsPerGen = 9;

	Model.HasElite = false;

	Model.HasHoF = false;

	Model.SelectMethod = Selection::FitnessUniform;
	Model.SelectChildMethod = Selection::Tour;
	Model.TournamentSize = 2;

	Model.CrossMethod = Crossover::KPoint;
	Model.KPointCount = 2;
	Model.CrossoverRate = 1.0f;//0.5f;

	Model.MutateMethod = Mutation::GaussianOffset;
	Model.GaussianOffset = 0.25f;
	Model.MutationRate = 0.0625f;//0.01f;

	Layer FeedbackLayer = Layer::Generational;

	Schedule->Add(Model, FeedbackLayer);

	Schedule->Run(false);*/

	system("pause");
	return 0;
}