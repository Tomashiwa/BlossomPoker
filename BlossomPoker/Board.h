#pragma once
#include <vector>
#include <iostream>
#include <array>

#include "BoardState.h"
#include "HandsEvaluator.h"

class Deck;
class Player;

class Board
{
public:
	Board(unsigned int _MinStack, unsigned int _MinBet);
	~Board();

	int HR[32487834];

	void InitEvaluator();
	int GetHandValue(int* _Cards);
	void Test();
	
	void Start();
	void Update();
	void End();

	void StartRound();
	void UpdateRound();
	void EndRound();

	void StartPhase();
	void UpdatePhase();
	void NextPhase();

	bool IsPhaseEnded();
	bool IsRoundEnded();
	bool IsGameEnded();

	void AddPlayer(Player* _AddingPlayer);
	void AddPlayer(Player* _AddingPlayer, unsigned int _EntryStack);
	void RemovePlayer(Player* _RemovingPlayer);

	Player* GetPreviousPlayer(Player* _Reference);
	Player* GetNextPlayer(Player* _Reference);

	void UpdatePot();
	void EmptyPot();
	void SplitPot(std::vector<unsigned int> &_Pots, std::vector<std::vector<Player*>> &_ValidPlayersPerPot);

	void DealCardsToPlayers();
	void IssueCommunalCards();
	void ClearCommunalCards();

	std::vector<Player*> GetNonBrokePlayers();
	std::vector<Player*> GetBettingPlayers();
	std::vector<Player*> DetermineWinningPlayers(std::vector<Player*> _Participants);
	void AwardPlayer(Player* _Player, unsigned int _Amt);

	void Print();

	bool GetIsActive() { return IsActive; }
	void SetActive(bool _IsActive) { IsActive = _IsActive; }

	BoardState GetState() { return CurrentState; }

	std::vector<Player*> GetPlayers() { return Players; }
	Player* GetDealingPlayer() { return DealingPlayer; }
	Player* GetSmallBlindPlayer() { return SmallBlindPlayer; }
	Player* GetBigBlindPlayer() { return BigBlindPlayer; }
	Player* GetCurrentPlayer() { return CurrentPlayer; }

	Deck* GetDeck() { return PlayingDeck; }
	std::array<Card*, 5>& GetCommunalCards() { return CommunalCards; }

	unsigned int GetPot() { return Pot; }
	unsigned int GetSmallBlind() { return SmallBlind; }
	unsigned int GetBigBlind() { return BigBlind; }
	unsigned int GetRequiredAnte() { return RequiredAnte; }

private:
	BoardState CurrentState;
	bool IsActive = false;

	std::vector<Player*> Players;
	Player* DealingPlayer = nullptr;
	Player* BigBlindPlayer = nullptr;
	Player* SmallBlindPlayer = nullptr;
	Player* CurrentPlayer = nullptr;

	Deck* PlayingDeck;
	std::array<Card*, 5> CommunalCards = { nullptr, nullptr, nullptr, nullptr, nullptr };

	unsigned int Pot = 0;
	unsigned int SmallBlind = 0;
	unsigned int BigBlind = 0;
	unsigned int RequiredAnte = 0; // Amount required to be called in order to stay on board

	unsigned int Round = 0;
	unsigned int EntryStack = BigBlind * 20;
};

