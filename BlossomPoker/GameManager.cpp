#include "GameManager.h"

#include "Table.h"
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

	for (unsigned int Index = 0; Index < Tables.size(); Index++)
		Tables[Index]->Start();
}

void GameManager::Update()
{
	bool AreAllTablesInactive = true;

	for (unsigned int Index = 0; Index < Tables.size(); Index++)
	{
		if (!Tables[Index]->GetIsActive()) continue;

		Tables[Index]->Update();
		AreAllTablesInactive = false;
	}

	if (AreAllTablesInactive)
		SetIsActive(false);
}

void GameManager::End()
{
	for (unsigned int Index = 0; Index < Tables.size(); Index++)
		Tables[Index]->End();
}

void GameManager::AddTable(unsigned int _StartBB)
{
	std::shared_ptr<Table> NewTable = std::make_shared<Table>(Evaluator, _StartBB, true);
	NewTable->AddPlayer(std::make_shared<Player>(NewTable, 0),_StartBB * 100);
	NewTable->AddPlayer(std::make_shared<Player>(NewTable, 1),_StartBB * 100);
	NewTable->SetActive(true);

	Tables.push_back(NewTable);
	std::cout << "New Table created (SB/BB: " << _StartBB/2 << "/" << _StartBB << " | Stack: " << _StartBB*100 << ") \n \n";
}
