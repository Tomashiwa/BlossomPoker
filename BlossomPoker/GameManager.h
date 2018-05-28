#pragma once

#include <iostream>
#include <vector>

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

	Board* NewBoard(unsigned int _MinStack, unsigned int _MinBet);

	void SetIsActive(bool _IsActive) { IsActive = _IsActive; }
	bool GetIsActive() { return IsActive; }

private:
	bool IsActive = true;
	std::vector<Board*> Boards;

};

