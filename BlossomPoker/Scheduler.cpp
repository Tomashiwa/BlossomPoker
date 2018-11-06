#include "Scheduler.h"

Scheduler::Scheduler()
{
	Test = std::make_unique<GeneticTest>();
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
		Test->SetSpecs(Entry->PopulationSize, Entry->GenerationLimit);

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

void Scheduler::Add(unsigned int _PopulationSize, unsigned int _GenerationLimit)
{
	Specs.push_back(std::make_unique<Specification>(_PopulationSize, _GenerationLimit));
}