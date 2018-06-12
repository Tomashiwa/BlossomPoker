#pragma once
#include <iostream>
#include <vector>

#include "HandEvaluator.h"

class Board;
class Player;

class GameManager
{
public:
	GameManager();
	~GameManager();

	void Start();
	void Update();
	void End();

	Board* NewBoard(unsigned int _StartBB);

	void SetIsActive(bool _IsActive) { IsActive = _IsActive; }
	bool GetIsActive() { return IsActive; }

	HandEvaluator* GetEvaluator() { return Evaluator; }

private:
	bool IsActive = true;
	std::vector<Board*> Boards;

	HandEvaluator* Evaluator = new HandEvaluator();

};

