#pragma once
#include <array>
#include <vector>
#include <string>
#include <memory>
#include "BettingAction.h"
#include "BlossomAI.h"

class Card;
class Table;

class Player
{
public:
	Player(std::shared_ptr<Table> _Table, unsigned int _Index);
	Player(std::shared_ptr<Table> _Table, unsigned int _Index, std::array<float,8> _Threshold);

	Player(const Player&) = delete;
	Player& operator= (const Player&) = delete;

	~Player();

	void Start();
	void Update();
	void End();
	void Reset();

	void SetHand(std::shared_ptr<Card> _First, std::shared_ptr<Card> _Second);
	void EmptyHand();

	void SetTable(std::shared_ptr<Table> _Table) { ResidingTable = _Table; }

	BettingAction DetermineAction();
	void SetAction(BettingAction _Action) { Action = _Action; }
	void GetAvaliableActions(std::vector<BettingAction>& _PossibleActions);

	const unsigned int GetIndex() { return Index; }

	std::array<std::shared_ptr<Card>, 2> GetHand() { return Hand; }
	std::shared_ptr<Card> GetHandCardByIndex(unsigned int _Index) { return Hand[_Index]; }

	unsigned int GetAnte() { return Ante; }
	unsigned int GetStack() { return Stack; }
	unsigned int GetPotContribution() { return PotContribution; }
	void SetAnte(unsigned int _Amt);
	void SetStack(unsigned int _Stack) { Stack = _Stack; }
	void SetPotContribution(unsigned int _PotContri) { PotContribution = _PotContri; }
	void EmptyPotContributon() { PotContribution = 0; }

	BettingAction GetAction() { return Action; }

	bool GetIsBroke() { return IsBroke; }
	bool GetIsFolded() { return IsFolded; }
	bool GetIsContributing() { return IsContributing; }
	void SetIsBroke(bool _IsBroke) { IsBroke = _IsBroke; }
	void SetIsFolded(bool _IsFolded) { IsFolded = _IsFolded; }
	void SetIsContributing(bool _IsContributing) { IsContributing = _IsContributing; }

	std::string GetHandInfo();

	BlossomAI& GetAI() { return *AI.get(); }

private:
	unsigned int Index;
	std::shared_ptr<BlossomAI> AI;
	std::shared_ptr<Table> ResidingTable;

	std::array<std::shared_ptr<Card>,2> Hand;
	
	unsigned int Ante = 0;
	unsigned int Stack = 0;
	unsigned int PotContribution = 0;

	BettingAction Action;

	bool IsBroke = false;
	bool IsFolded = false;
	bool IsContributing = true;
};

