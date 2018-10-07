#pragma once
#include <iostream>
#include <vector>
#include <memory>

#include "HandEvaluator.h"

class Board;
class Player;

class GameManager
{
public:
	GameManager();
	
	GameManager(const GameManager&) = delete;
	GameManager& operator= (const GameManager&) = delete;
	
	~GameManager();

	void Start();
	void Update();
	void End();

	void AddBoard(unsigned int _StartBB);

	void SetIsActive(bool _IsActive) { IsActive = _IsActive; }
	bool GetIsActive() { return IsActive; }
	bool GetIsPrintingRoundInfo() { return IsPrintingRoundInfo; }

	std::shared_ptr<HandEvaluator> GetEvaluator() { return Evaluator; }

private:
	bool IsActive = true;
	bool IsPrintingRoundInfo;

	std::vector<std::shared_ptr<Board>> Boards;
	std::shared_ptr<HandEvaluator> Evaluator = std::make_shared<HandEvaluator>();

};

