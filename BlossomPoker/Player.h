#pragma once
#include <array>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <memory>
#include "BettingAction.h"
#include "BlossomAI.h"

class Card;
class Table;

class Player
{
public:
	Player(const std::shared_ptr<Table>& _Table, unsigned int _Index);

	Player(const Player&) = delete;
	Player& operator= (const Player&) = delete;

	~Player();

	void Start();
	void Update();
	void End();
	void Reset();

	void SetHand(const std::shared_ptr<Card>& _First, const std::shared_ptr<Card>& _Second);
	std::string GetHandInfo();
	void EmptyHand();

	void GetAvaliableActions(std::vector<BettingAction>& _PossibleActions);
	BettingAction DetermineAction();

	const unsigned int GetIndex() { return Index; }
	unsigned int GetStack() { return Stack; }
	unsigned int GetAnte() { return Ante; }
	unsigned int GetPotContribution() { return PotContribution; }
	void SetStack(unsigned int _Stack) { Stack = _Stack; }
	void SetAnte(unsigned int _Amt);
	void SetPotContribution(unsigned int _PotContri) { PotContribution = _PotContri; }
	void EmptyPotContributon() { PotContribution = 0; }
	
	bool GetIsContributing() { return IsContributing; }
	bool GetIsFolded() { return IsFolded; }
	bool GetIsBroke() { return IsBroke; }
	void SetIsContributing(bool _IsContributing) { IsContributing = _IsContributing; }
	void SetIsFolded(bool _IsFolded) { IsFolded = _IsFolded; }
	void SetIsBroke(bool _IsBroke) { IsBroke = _IsBroke; }
	
	BettingAction GetAction() { return CurrentAction; }
	void SetAction(BettingAction _Action) { CurrentAction = _Action; }
	std::array<std::shared_ptr<Card>, 2> GetHand() { return Hand; }
	std::shared_ptr<Card> GetHandCardByIndex(unsigned int _Index) { return Hand[_Index]; }

	void SetTable(std::shared_ptr<Table> _Table) { ResidingTable = _Table; }

	BlossomAI& GetAI() { return *AI.get(); }

private:
	unsigned int Index;
	unsigned int Stack = 0;
	unsigned int Ante = 0;
	unsigned int PotContribution = 0;

	bool IsContributing = true;
	bool IsFolded = false;
	bool IsBroke = false;
	
	BettingAction CurrentAction;
	std::array<std::shared_ptr<Card>,2> Hand;
	
	std::shared_ptr<Table> ResidingTable;
	
	std::shared_ptr<BlossomAI> AI;
};

