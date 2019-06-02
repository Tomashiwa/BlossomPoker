#pragma once
#include <array>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <memory>

#include "../../Table/inc/BettingAction.h"
#include "../../AI/inc/BlossomAI.h"

class Card;
class Table;

class Player
{
public:
	Player(const std::shared_ptr<Table>& _Table, unsigned int _Index);

	virtual void Initialize();
	virtual void Update();
	virtual void End();
	virtual void Reset();

	std::string GetHandInfo();
	virtual void ClearStats();

	virtual float CalculateFitness();
	virtual float CalculateEarnings();

	virtual BettingAction DetermineAction() = 0;

	//Getters
	const unsigned int GetIndex() { return Index; }

	bool GetIsInQuestion() { return IsInQuestion; }
	bool GetIsInHoF() { return IsInHoF; }

	unsigned int GetStack() { return Stack; }
	unsigned int GetAnte() { return Ante; }
	unsigned int GetPotContribution() { return PotContribution; }
	virtual unsigned int GetRaiseBetAmt() { return RaiseBetAmt; }

	bool GetIsContributing() { return IsContributing; }
	bool GetIsFolded() { return IsFolded; }
	bool GetIsBroke() { return IsBroke; }

	unsigned int GetMoneyWon() { return MoneyWon; }
	unsigned int GetMoneyLost() { return MoneyLost; }
	unsigned int GetHandsWon() { return HandsWon; }
	unsigned int GetHandsLost() { return HandsLost; }
	int GetProfits() { return Profits; }

	int GetRanking() { return Ranking; }
	unsigned int GetMatchesJoined() { return MatchesJoined; }

	float GetEarnings() { return Earnings; }
	float GetFitness() { return Fitness; }

	std::array<Card, 2> GetHand() { return Hand; }
	Card GetHandCardByIndex(unsigned int _Index) { return Hand[_Index]; }

	BettingAction GetAction() { return CurrentAction; }
	void GetAvaliableActions(std::vector<BettingAction>& _PossibleActions);

	std::shared_ptr<Table> GetTable() { return ResidingTable; }

	//Setters
	void SetRanking(int _Ranking) { Ranking = _Ranking; }
	void SetMatchesJoined(int _Matches) { MatchesJoined = _Matches; }

	void SetInQuestion(bool _InQuestion) { IsInQuestion = _InQuestion; }
	void SetInHoF(bool _InHoF) { IsInHoF = _InHoF; }

	void SetStack(unsigned int _Stack) { Stack = _Stack; }
	void SetAnte(unsigned int _Amt);
	void SetPotContribution(unsigned int _PotContri) { PotContribution = _PotContri; }
	void EmptyPotContributon() { PotContribution = 0; }

	void SetIsContributing(bool _IsContributing) { IsContributing = _IsContributing; }
	void SetIsFolded(bool _IsFolded) { IsFolded = _IsFolded; }
	void SetIsBroke(bool _IsBroke) { IsBroke = _IsBroke; }

	void SetMoneyWon(unsigned int _MoneyWon) { MoneyWon = _MoneyWon; }
	void SetMoneyLost(unsigned int _MoneyLost) { MoneyLost = _MoneyLost; }
	void SetHandsWon(unsigned int _HandsWon) { HandsWon = _HandsWon; }
	void SetHandsLost(unsigned int _HandsLost) { HandsLost = _HandsLost; }
	void SetProfits(unsigned int _Profits) { Profits = _Profits; }

	void SetHand(Card& _First, Card& _Second);

	void SetAction(BettingAction _Action) { CurrentAction = _Action; }

	void SetTable(std::shared_ptr<Table> _Table) { ResidingTable = _Table; }

protected:
	unsigned int Index;

	std::array<Card, 2> Hand;
	BettingAction CurrentAction;

	bool IsInQuestion = false;
	bool IsInHoF = false;

	bool IsContributing = true;
	bool IsFolded = false;
	bool IsBroke = false;

	int Ranking = -1;
	unsigned int MatchesJoined = 0;

	unsigned int Stack = 0;
	unsigned int Ante = 0;
	unsigned int PotContribution = 0;
	unsigned int RaiseBetAmt = 0;

	unsigned int MoneyWon = 0;
	unsigned int MoneyLost = 0;
	unsigned int HandsWon = 0;
	unsigned int HandsLost = 0;

	int Profits = 0;
	float Earnings = 0.0f;

	float Fitness = 0.0f;

	std::shared_ptr<Table> ResidingTable;
};

