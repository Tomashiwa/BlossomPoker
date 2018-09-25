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
		Test->Reset();
		Test->SetSpecs(Entry->PopulationSize, Entry->SubjectsAmt, Entry->GenerationLimit);

		Test->Start();

		while (!Test->IsTestComplete())
			Test->Update();

		Test->End();
	}

	End();
}

void Scheduler::Start()
{

}

void Scheduler::End()
{

}

void Scheduler::Add(unsigned int _PopulationSize, unsigned int _SubjectsSize, unsigned int _GenerationLimit)
{
	Specs.push_back(std::make_unique<Specification>(_PopulationSize, _SubjectsSize, _GenerationLimit));
}