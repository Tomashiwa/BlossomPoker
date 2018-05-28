#include <iostream>
#include <string>
#include "GameManager.h"

int main()
{
	GameManager* Manager = new GameManager();

	Manager->NewBoard(20, 5);
	Manager->Start();

	while (Manager->GetIsActive())
	{
		Manager->Update();

		system("pause");
		std::cout << "========================================" << std::endl;
	}

	Manager->End();
	system("pause");

	return 0;
}
