#include <iostream>
#include <string>
#include <memory>

#include "GeneticTrainer.h"
#include "Scheduler.h"

int main()
{
	std::unique_ptr<Scheduler> Schedule = std::make_unique<Scheduler>();

	Schedule->Add(8, 100, 8);
	
	Schedule->Run();
	system("pause");

	return 0;
}