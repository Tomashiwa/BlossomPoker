#include "GameManager.h"

#include "Board.h"
#include "Player.h"
#include "HandEvaluator.h"

GameManager::GameManager()
{
}

GameManager::~GameManager()
{
}

void GameManager::Start()
{
	IsPrintingRoundInfo = true;

	for (unsigned int Index = 0; Index < Boards.size(); Index++)
		Boards[Index]->Start();
}

void GameManager::Update()
{
	bool AreAllBoardInactive = true;

	for (unsigned int Index = 0; Index < Boards.size(); Index++)
	{
		if (!Boards[Index]->GetIsActive()) continue;

		Boards[Index]->Update();
		AreAllBoardInactive = false;
	}

	if (AreAllBoardInactive)
		SetIsActive(false);
}

void GameManager::End()
{
	for (unsigned int Index = 0; Index < Boards.size(); Index++)
		Boards[Index]->End();
}

Board* GameManager::NewBoard(unsigned int _StartBB)
{
	Board* _NewBoard = new Board(this, _StartBB);
	_NewBoard->AddPlayer(new Player(_NewBoard, 0),_StartBB * 100);
	_NewBoard->AddPlayer(new Player(_NewBoard, 1),_StartBB * 100);
	_NewBoard->SetActive(true);

	Boards.push_back(_NewBoard);
	std::cout << "New Board created (SB/BB: " << _StartBB/2 << "/" << _StartBB << " | Stack: " << _StartBB*100 << ") \n \n";

	return _NewBoard;
}
