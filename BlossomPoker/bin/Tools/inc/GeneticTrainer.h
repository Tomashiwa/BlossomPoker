#pragma once
#include <vector>
#include <chrono>
#include <random>
#include <memory>
#include <time.h>

#include "../../Table/inc/Phase.h"
#include "../inc/LogWriter.h"

class Table;
class Player;
class BlossomPlayer;
class Participant;
class Folder;
class Caller;
class Raiser;
class Randomer;
class Tournament;
class HandEvaluator;

class GeneticTrainer
{
public:
	GeneticTrainer();
	~GeneticTrainer();

	void Start();
	void Run();
	void End();
	void Reset();

	void SetSpecs(unsigned int _PopulationSize, unsigned int _GenerationLimit, unsigned int _ToursPerGen);

	bool IsTestComplete();
	
	std::string GetPopulationContentStr();
	std::string GetThresholdsStr(const std::shared_ptr<BlossomPlayer>& _Target);

private:
	unsigned int PopulationSize;
	unsigned int GenerationLimit;
	unsigned int ToursPerGen;
	unsigned int TableSize = 8;
	
	unsigned int Generation = 0;
	unsigned int PlayersGenerated = 0;

	float CrossoverRate = 0.7f;
	float MutateDelta = 0.25f;
	float MutateRate = 0.1f;

	unsigned int ParentLimit = 2;
	unsigned int TournamentSize = 2;
	unsigned int WinnerPerTouranment = 1;

	std::vector<std::shared_ptr<BlossomPlayer>> Population;
	std::vector<std::shared_ptr<Player>> PlayingPopulation;
	std::vector<std::shared_ptr<Participant>> RankingBoard;

	std::shared_ptr<Table> ActiveTable;
	std::vector<std::unique_ptr<Tournament>> Tournaments;
	std::vector<std::shared_ptr<Participant>> HoF;

	//std::shared_ptr<Folder> FoldingPlayer;
	std::shared_ptr<Caller> CallingPlayer;
	std::shared_ptr<Raiser> RaisingPlayer;
	std::shared_ptr<Randomer> RandomPlayer;
	std::shared_ptr<Randomer> RandomPlayer1;

	std::mt19937 MTGenerator;
	std::shared_ptr<HandEvaluator> Evaluator;
	std::unique_ptr<LogWriter> Writer;

	void GeneratePopulation(unsigned int _Size);

	void RankPlayer(const std::shared_ptr<BlossomPlayer>& _Player);
	void ArrangePlayers(std::vector<std::shared_ptr<BlossomPlayer>>& _Players);
	void ArrangeHoF();

	const std::shared_ptr<Participant>& GetParticipant(unsigned int _Index);
	const std::shared_ptr<Player>& GetBestPlayer();
	
	std::shared_ptr<BlossomPlayer>& TournamentSelect(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation);
	void Crossover(const std::shared_ptr<BlossomPlayer>& _First, const std::shared_ptr<BlossomPlayer>& _Second, std::vector<std::shared_ptr<BlossomPlayer>>& _Results);
	void Mutate(std::shared_ptr<BlossomPlayer>& _Target, Phase _Phase);
	//void Mutate(std::shared_ptr<BlossomPlayer>& _Target, Phase _Phase, unsigned int _Index);
	void ReproducePopulation();

	bool HasCrossoverHappen();

	void EvaluateMutateRate();
	bool HasMutationHappen();

	float GetOverallFitness();
	float GetGenerationDiversity();
};

