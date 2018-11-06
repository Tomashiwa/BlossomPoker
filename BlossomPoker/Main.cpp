#include <iostream>
#include <string>
#include <memory>

//#include "GameManager.h"
#include "GeneticTest.h"
#include "Scheduler.h"

int main()
{
	std::unique_ptr<Scheduler> Schedule = std::make_unique<Scheduler>();

	Schedule->Add(8, 50);

	Schedule->Run();
	system("pause");

	return 0;
}
