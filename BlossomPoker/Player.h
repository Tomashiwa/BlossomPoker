#pragma once
#include <array>
#include <vector>
#include <string>
#include <memory>
#include "BettingAction.h"
#include "BlossomAI.h"

class Card;
class Board;

class Player
{
public:
	Player(std::shared_ptr<Board> _Board, unsigned int _Index);
	Player(std::shared_ptr<Board> _Board, unsigned int _Index, std::array<double,8> _Threshold);

	Player(const Player&) = delete;
	Player& operator= (const Player&) = delete;

	~Player();

	void Start();
	void Update();
	void End();
	void Reset();

	void SetBoard(std::shared_ptr<Board> _Board);
	void SetHand(std::shared_ptr<Card> _First, std::shared_ptr<Card> _Second);

	void EmptyHand();

	void SetStack(unsigned int _Amt);
	void SetAnte(unsigned int _Amt);
	void EmptyPotContributon();

	BettingAction DetermineAction();
	void SetAction(BettingAction _Action);
	void GetAvaliableActions(std::vector<BettingAction>& _PossibleActions);

	void SetIsBroke(bool _IsBroke);
	void SetIsFolded(bool _IsFolded);
	void SetIsParticipating(bool _IsParticipating);

	std::array<std::shared_ptr<Card>, 2> GetHand() { return Hand; }
	std::shared_ptr<Card> GetHandCardByIndex(unsigned int _Index);
	const unsigned int GetIndex() { return Index; }

	unsigned int GetAnte() { return Ante; }
	unsigned int GetStack() { return Stack; }
	unsigned int GetPotContribution() { return PotContribution; }

	BettingAction GetAction() { return Action; }

	bool GetIsBroke() { return IsBroke; }
	bool GetIsFolded() { return IsFolded; }
	bool GetIsParticipating() { return IsParticipating; }

	std::string GetHandInfo();

	BlossomAI& GetAI() { return *AI.get(); }

private:
	unsigned int Index;
	std::shared_ptr<BlossomAI> AI;
	std::shared_ptr<Board> SelfBoard;

	std::array<std::shared_ptr<Card>,2> Hand;
	
	unsigned int Ante = 0;
	unsigned int Stack = 0;
	unsigned int PotContribution = 0;

	BettingAction Action;

	bool IsBroke = false;
	bool IsFolded = false;
	bool IsParticipating = true;
};

