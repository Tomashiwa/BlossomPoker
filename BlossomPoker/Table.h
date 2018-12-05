#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <array>
#include <map>

#include "Phase.h"
#include "Match.h"
#include "Tournament.h"

class Card;
class Deck;
class Player;
class HandEvaluator;

class Table
{
public:
	Table(std::shared_ptr<HandEvaluator> _Evaluator, unsigned int _BigBlind, bool _PrintProcess);

	Table(const Table&) = delete;
	Table& operator= (const Table&) = delete;

	~Table();

	int HR[32487834];

	void InitEvaluator();
	int GetHandValue(int* _Cards);
	
	void Run();

	void Start();
	void Update();
	void End();

	void Reset(bool _IsHard);

	void StartRound();
	void UpdateRound();
	void EndRound();

	void StartPhase();
	void UpdatePhase();
	void NextPhase();	

	bool IsPhaseEnded();
	bool IsRoundEnded();
	bool IsGameEnded();

	void AddPlayer(std::shared_ptr<Player> _AddingPlayer);
	void AddPlayer(std::shared_ptr<Player> _AddingPlayer, unsigned int _EntryStack);
	void RemoveAllPlayers();

	std::shared_ptr<Player> GetPreviousPlayer(const std::shared_ptr<Player>& _Reference);
	std::shared_ptr<Player> GetNextPlayer(const std::shared_ptr<Player>& _Reference);

	void UpdatePot();
	void EmptyPot();
	void SplitPot(std::vector<unsigned int>& _Pots, std::vector<std::vector<std::shared_ptr<Player>>>& _ContributorsPerPot, std::vector<std::vector<std::shared_ptr<Player>>>& _ContestantsPerPot);

	void DistributeWinnings();

	void RestockDeck();
	void SaveDeckArrangement();
	void LoadDeckArrangement();
	void DealCardsToPlayers();
	void IssueCommunalCards();
	void ClearCommunalCards();

	void ShiftDealer(const std::shared_ptr<Player>& _Target);

	unsigned int GetEntryIndex(const std::shared_ptr<Player>& _Target);
	int GetPlayerEarnings(const std::shared_ptr<Player>& _Target);

	void GetParticipatingPlayers(std::vector<std::shared_ptr<Player>>& _Participants);
	void DetermineWinningPlayers(const std::vector<std::shared_ptr<Player>>& _Participants, std::vector<std::shared_ptr<Player>>& _Winners);
	void DetermineWinningPlayers(std::vector<std::shared_ptr<Participant>>& _Participants, std::vector<std::shared_ptr<Participant>>& _Winners);
	void AwardPlayer(const std::shared_ptr<Player>& _Player, unsigned int _Amt);

	void RankPlayers(std::vector<int>& _Rankings);

	void Print();

	void SetMatch(const std::shared_ptr<Match>& _NewMatch);

	std::shared_ptr<Match> GetCurrentMatch() { return CurrentMatch; }

	std::string GetStateStr();

	bool GetIsActive() { return IsActive; }
	void SetActive(bool _IsActive) { IsActive = _IsActive; }

	Phase GetState() { return CurrentState; }

	std::shared_ptr<HandEvaluator> GetEvaluator() { return Evaluator; }

	std::vector<std::shared_ptr<Player>> GetPlayers() { return Players; }
	std::shared_ptr<Player> GetDealingPlayer() { return DealingPlayer; }
	std::shared_ptr<Player> GetSmallBlindPlayer() { return SmallBlindPlayer; }
	std::shared_ptr<Player> GetBigBlindPlayer() { return BigBlindPlayer; }
	std::shared_ptr<Player> GetCurrentPlayer() { return CurrentPlayer; }
	std::shared_ptr<Player> GetFirstPlayer() { return FirstPlayer; }

	std::array<std::shared_ptr<Card>, 5> GetCommunalCards() { return CommunalCards; }
	std::shared_ptr<Card> GetCommunalCardByIndex(unsigned int _Index) { return CommunalCards[_Index]; }

	unsigned int GetPot() { return Pot; }
	unsigned int GetSmallBlind() { return SmallBlind; }
	unsigned int GetBigBlind() { return BigBlind; }
	unsigned int GetRequiredAnte() { return RequiredAnte; }
	unsigned int GetEntryStack() { return EntryStack; }

	unsigned int GetRounds() { return Round; }

private:
	std::shared_ptr<HandEvaluator> Evaluator;

	std::shared_ptr<Match> CurrentMatch;

	Phase CurrentState;

	std::vector<std::shared_ptr<Player>> Players;
	std::shared_ptr<Player> DealingPlayer = nullptr;
	std::shared_ptr<Player> BigBlindPlayer = nullptr;
	std::shared_ptr<Player> SmallBlindPlayer = nullptr;
	std::shared_ptr<Player> CurrentPlayer = nullptr;
	std::shared_ptr<Player> FirstPlayer = nullptr;

	std::unique_ptr<Deck> ActiveDeck;
	std::unique_ptr<Deck> ArrangedDeck;
	std::array<std::shared_ptr<Card>, 5> CommunalCards;

	unsigned int Round = 0;
	unsigned int EntryStack = BigBlind * 20;

	unsigned int Pot = 0;
	unsigned int SmallBlind = 0;
	unsigned int BigBlind = 0;
	unsigned int RequiredAnte = 0;

	bool IsActive = false;
	bool PrintProcess = false;
};

