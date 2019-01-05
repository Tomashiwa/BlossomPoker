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

	virtual void Start();
	virtual void Update();
	virtual void End();
	virtual void Reset();

	void SetHand(Card& _First, Card& _Second);
	std::string GetHandInfo();

	void GetAvaliableActions(std::vector<BettingAction>& _PossibleActions);
	virtual BettingAction DetermineAction() = 0;

	const unsigned int GetIndex() { return Index; }
	unsigned int GetStack() { return Stack; }
	unsigned int GetAnte() { return Ante; }
	unsigned int GetPotContribution() { return PotContribution; }
	virtual unsigned int GetRaiseBetAmt() { return RaiseBetAmt; }
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
	std::array<Card, 2> GetHand() { return Hand; }
	Card GetHandCardByIndex(unsigned int _Index) { return Hand[_Index]; }

	void SetTable(std::shared_ptr<Table> _Table) { ResidingTable = _Table; }

protected:
	unsigned int Index;
	unsigned int Stack = 0;
	unsigned int Ante = 0;
	unsigned int PotContribution = 0;

	unsigned int RaiseBetAmt = 0;

	bool IsContributing = true;
	bool IsFolded = false;
	bool IsBroke = false;
	
	BettingAction CurrentAction;
	std::array<Card,2> Hand;
	
	std::shared_ptr<Table> ResidingTable;
};

