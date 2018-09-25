#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <array>
#include <map>

#include "Phase.h"

class Card;
class Deck;
class Player;
class HandEvaluator;

class Board
{
public:
	Board(std::shared_ptr<HandEvaluator> _Evaluator, unsigned int _BigBlind, bool _PrintProcess);

	Board(const Board&) = delete;
	Board& operator= (const Board&) = delete;

	~Board();

	int HR[32487834];

	void InitEvaluator();
	int GetHandValue(int* _Cards);
	void Test();

	void Start();
	void Update();
	void End();

	void Reset(bool _IsHard);
	void SoftReset();
	void ResetRecords();
	void ResetStacks();

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
	void RemovePlayer(std::shared_ptr<Player>& _RemovingPlayer);
	void RemoveAllPlayers();

	std::shared_ptr<Player> GetPreviousPlayer(const std::shared_ptr<Player>& _Reference);
	std::shared_ptr<Player> GetNextPlayer(const std::shared_ptr<Player>& _Reference);

	void UpdatePot();
	void EmptyPot();
	void SplitPot(std::vector<unsigned int> &_Pots, std::vector<std::vector<std::shared_ptr<Player>>> &_ValidPlayersPerPot);

	void DealCardsToPlayers();
	void IssueCommunalCards();
	void ClearCommunalCards();

	void GetParticipatingPlayers(std::vector<std::shared_ptr<Player>>& _Participants);
	void GetBettingPlayers(std::vector<std::shared_ptr<Player>>& _BettingPlayers);
	void DetermineWinningPlayers(const std::vector<std::shared_ptr<Player>>& _Participants, std::vector<std::shared_ptr<Player>>& _Winners);
	void AwardPlayer(const std::shared_ptr<Player>& _Player, unsigned int _Amt);

	void Print();
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

	unsigned int GetPlayerWins(const std::shared_ptr<Player>& _Target) { return Records[_Target]; }

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

	Phase CurrentState;
	bool IsActive = false;

	std::vector<std::shared_ptr<Player>> Players;
	std::shared_ptr<Player> DealingPlayer = nullptr;
	std::shared_ptr<Player> BigBlindPlayer = nullptr;
	std::shared_ptr<Player> SmallBlindPlayer = nullptr;
	std::shared_ptr<Player> CurrentPlayer = nullptr;

	std::map<std::shared_ptr<Player>, unsigned int> Records;

	std::unique_ptr<Deck> PlayingDeck;
	std::array<std::shared_ptr<Card>, 5> CommunalCards;

	unsigned int Pot = 0;
	unsigned int SmallBlind = 0;
	unsigned int BigBlind = 0;
	unsigned int RequiredAnte = 0; // Amount required to be called in order to stay on board

	unsigned int Round = 0;
	unsigned int EntryStack = BigBlind * 20;

	bool PrintProcess = false;
};

