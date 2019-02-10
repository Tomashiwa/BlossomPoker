#include <iostream>
#include <string>
#include <memory>

#include "../bin/Tools/inc/GeneticTrainer.h"
#include "../bin/Tools/inc/Scheduler.h"

int main()
{
	std::unique_ptr<Scheduler> Schedule = std::make_unique<Scheduler>();

	Schedule->Add(32, 300, 32);

	Schedule->Run(false);
	system("pause");

	return 0;
}