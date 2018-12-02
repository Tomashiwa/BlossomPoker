#include <iostream>
#include <string>
#include <memory>

#include "GeneticTest.h"
#include "Scheduler.h"

int main()
{
	std::unique_ptr<Scheduler> Schedule = std::make_unique<Scheduler>();

	Schedule->Add(8, 10, 5);
	
	Schedule->Run();
	system("pause");

	return 0;
}
