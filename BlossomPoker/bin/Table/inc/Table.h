#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <array>
#include <map>

#include "../../Cards/inc/Deck.h"
#include "../inc/Phase.h"
#include "../inc/Match.h"
#include "../inc/Tournament.h"

class Card;
class Player;
class HandEvaluator;

class Table
{
public:
	Table(std::shared_ptr<HandEvaluator> _Evaluator, unsigned int _BigBlind, bool _PrintProcess);

	void Run(bool _IsTraining);

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
	
	void ShiftDealer(const std::shared_ptr<Player>& _Target);

	void GetActivePlayers(std::vector<std::shared_ptr<Player>>& _ActivePlayers);
	void GetParticipatingPlayers(std::vector<std::shared_ptr<Player>>& _Participants);
	void DetermineWinningPlayers(const std::vector<std::shared_ptr<Player>>& _Participants, std::vector<std::shared_ptr<Player>>& _Winners);
	void DetermineWinningPlayers(std::vector<std::shared_ptr<Participant>>& _Participants, std::vector<std::shared_ptr<Participant>>& _Winners);
	void AwardPlayer(const std::shared_ptr<Player>& _Player, unsigned int _Amt);

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

	std::vector<Card> GetCommunalCards() { return CommunalCards; }
	Card& GetCommunalCardByIndex(unsigned int _Index) { return CommunalCards[_Index]; }

	unsigned int GetPot() { return Pot; }
	unsigned int GetSmallBlind() { return SmallBlind; }
	unsigned int GetBigBlind() { return BigBlind; }
	unsigned int GetRequiredAnte() { return RequiredAnte; }
	unsigned int GetEntryStack() { return EntryStack; }

	unsigned int GetPrevRaiseBet() { return PrevRaiseBet; }

	unsigned int GetRounds() { return Round; }

	bool GetIsTraining() { return IsTraining; }

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

	Deck ActiveDeck;
	Deck ArrangedDeck;
	std::vector<Card> CommunalCards;

	unsigned int Round = 0;
	unsigned int EntryStack = BigBlind * 20;

	unsigned int Pot = 0;
	unsigned int SmallBlind = 0;
	unsigned int BigBlind = 0;
	unsigned int RequiredAnte = 0;

	unsigned int PrevRaiseBet = 0;

	bool IsActive = false;
	bool IsTraining = true;
	bool PrintProcess = false;
};

