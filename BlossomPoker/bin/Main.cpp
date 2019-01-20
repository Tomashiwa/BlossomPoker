#include <iostream>
#include <string>
#include <memory>

#include "../bin/Tools/inc/GeneticTrainer.h"
#include "../bin/Tools/inc/Scheduler.h"

int main()
{
	std::unique_ptr<Scheduler> Schedule = std::make_unique<Scheduler>();

	//for(unsigned int Index = 0; Index < 10; Index++)
		Schedule->Add(16, 100, 16);

	Schedule->Run(false);
	system("pause");

	return 0;
}