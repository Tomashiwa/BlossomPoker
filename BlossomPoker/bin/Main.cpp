#include <iostream>
#include <string>
#include <memory>

#include "../bin/Tools/inc/GeneticTrainer.h"
#include "../bin/Tools/inc/Scheduler.h"

int main()
{
	std::unique_ptr<Scheduler> Schedule = std::make_unique<Scheduler>();

	Schedule->Add(8, 1, 1);

	Schedule->Run(true);
	system("pause");

	return 0;
}