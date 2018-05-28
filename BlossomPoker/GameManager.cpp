#include "GameManager.h"

#include "Board.h"
#include "Player.h"

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

void GameManager::Start()
{
	for (unsigned int Index = 0; Index < Boards.size(); Index++)
	{
		//std::cout << "Board " << Index << " is starting..." << std::endl;
		std::cout << "========================================" << std::endl;
		Boards[Index]->Start();
		std::cout << "========================================" << std::endl;
	}
}

void GameManager::Update()
{
	bool AreAllBoardInactive = true;

	for (unsigned int Index = 0; Index < Boards.size(); Index++)
	{
		Boards[Index]->Update();

		if (!Boards[Index]->GetIsActive()) continue;
		
		Boards[Index]->Print();

		std::cout << "========================================" << std::endl;

		AreAllBoardInactive = false;
	}

	if (AreAllBoardInactive)
		SetIsActive(false);
}

void GameManager::End()
{
	for (unsigned int Index = 0; Index < Boards.size(); Index++)
	{
		std::cout << "========================================" << std::endl;
		Boards[Index]->End();
		std::cout << "========================================" << std::endl;
	}
}

Board* GameManager::NewBoard(unsigned int _MinStack, unsigned int _MinBet)
{
	std::cout << "A new Board has been created... (MinStack: " << _MinStack << " / MinBet: " << _MinBet << ")" << std::endl;

	Board* _NewBoard = new Board(_MinStack, _MinBet);
	_NewBoard->AddPlayer(new Player(_NewBoard, 0),500);
	_NewBoard->AddPlayer(new Player(_NewBoard, 1),500);
	_NewBoard->SetActive(true);

	Boards.push_back(_NewBoard);

	return _NewBoard;
}
