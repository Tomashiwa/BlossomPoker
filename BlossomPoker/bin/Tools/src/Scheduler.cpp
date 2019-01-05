#include "../inc/Scheduler.h"

Scheduler::Scheduler()
{
	Test = std::make_unique<GeneticTrainer>();
}

Scheduler::~Scheduler()
{
}

void Scheduler::Run()
{
	Start();

	for(auto const& Entry :Specs)
	{
		std::cout << "Running Specification Set (PopSize: " << Entry->PopulationSize << ", GenLimit: " << Entry->GenerationLimit << ")...\n\n";

		Test->Reset();
		Test->SetSpecs(Entry->PopulationSize, Entry->GenerationLimit, Entry->ToursPerGen);

		Test->Run();
	}

	End();
}

void Scheduler::Start()
{

}

void Scheduler::End()
{

}

void Scheduler::Add(unsigned int _PopulationSize, unsigned int _GenerationLimit, unsigned int _ToursPerGen)
{
	Specs.push_back(std::make_unique<Specification>(_PopulationSize, _GenerationLimit, _ToursPerGen));
}