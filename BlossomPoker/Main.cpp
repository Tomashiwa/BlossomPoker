#include <iostream>
#include <string>
#include "GameManager.h"
#include "GeneticTest.h"

int main()
{
	GeneticTest* Test = new GeneticTest();

	Test->Start();

	while (!Test->IsTestComplete())
		Test->Update();

	Test->End();
	system("pause");

	//GameManager* Manager = new GameManager();

	//Manager->NewBoard(20);
	//Manager->Start();

	//while (Manager->GetIsActive())
	//{
	//	Manager->Update();
	//	//system("pause");
	//	//std::cout << "\n";
	//}

	//Manager->End();
	//system("pause");

	return 0;
}
