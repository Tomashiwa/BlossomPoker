#include <iostream>
#include <string>
#include <memory>

#include "GameManager.h"
#include "GeneticTest.h"

int main()
{
	std::unique_ptr<GeneticTest> Test = std::make_unique<GeneticTest>();

	Test->Start();

	while (!Test->IsTestComplete())
		Test->Update();

	Test->End();
	system("pause");

	//GameManager* Manager = new GameManager();

	//Manager->AddBoard(20);
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
