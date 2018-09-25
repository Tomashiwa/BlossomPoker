#include <iostream>
#include <string>
#include <memory>

//#include "GameManager.h"
#include "GeneticTest.h"
#include "Scheduler.h"

int main()
{
	std::unique_ptr<Scheduler> Schedule = std::make_unique<Scheduler>();

	/*Schedule->Add(8, 10, 50);
	Schedule->Add(16, 10, 50);
	Schedule->Add(32, 10, 50);
	Schedule->Add(50, 10, 50);*/

	/*Schedule->Add(8, 25, 50);
	Schedule->Add(16, 25, 50);
	Schedule->Add(32, 25, 50);*/
	//Schedule->Add(50, 25, 50);

	Schedule->Add(8, 50, 50);
	Schedule->Add(16, 50, 50);
	/*Schedule->Add(32, 50, 50);
	Schedule->Add(50, 50, 50);*/

	Schedule->Run();
	system("pause");

	/*std::unique_ptr<GeneticTest> Test = std::make_unique<GeneticTest>();

	Test->Start();

	while (!Test->IsTestComplete())
		Test->Update();

	Test->End();
	system("pause");*/

	return 0;
}
