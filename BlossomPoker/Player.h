#pragma once
#include <array>
#include <vector>
#include <string>
#include "BettingAction.h"
#include "BlossomAI.h"

class Card;
class Board;

class Player
{
public:
	Player(Board* _Board, unsigned int _Index);
	Player(Board* _Board, unsigned int _Index, std::array<double,8> _Threshold);

	~Player();

	void Start();
	void Update();
	void End();

	void Reset();

	void SetBoard(Board* _Board);

	void EmptyHand();
	void SetHand(Card* _First, Card* _Second);

	void SetStack(unsigned int _Amt);
	void SetAnte(unsigned int _Amt);

	void EmptyPotContributon();

	std::vector<BettingAction> GetAvaliableActions();
	BettingAction DetermineAction();
	void SetAction(BettingAction _Action);

	void SetIsParticipating(bool _IsParticipating);
	void SetIsFolded(bool _IsFolded);
	void SetIsBroke(bool _IsBroke);

	unsigned int GetIndex() { return Index; }
	std::array<Card*,2> GetHand() { return Hand; }

	unsigned int GetStack() { return Stack; }
	unsigned int GetAnte() { return Ante; }
	unsigned int GetPotContribution() { return PotContribution; }

	BettingAction GetAction() { return Action; }

	bool GetIsParticipating() { return IsParticipating; }
	bool GetIsFolded() { return IsFolded; }
	bool GetIsBroke() { return IsBroke; }

	std::string GetHandInfo();

	BlossomAI* GetAI() { return AI; }

private:
	unsigned int Index;
	Board* SelfBoard;
	BlossomAI* AI;

	std::array<Card*,2> Hand;
	
	unsigned int Stack = 0;
	unsigned int Ante = 0;
	unsigned int PotContribution = 0;

	BettingAction Action;

	bool IsParticipating = true;
	bool IsFolded = false;
	bool IsBroke = false;
};

