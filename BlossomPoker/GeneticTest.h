#pragma once
#include <vector>
#include <chrono>
#include <random>
#include <memory>
#include <time.h>

#include "Phase.h"
#include "LogWriter.h"

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

class GeneticTest
{
public:
	GeneticTest();
	~GeneticTest();

	void Start();
	void Run();
	void End();
	void Reset();

	void SetSpecs(unsigned int _PopulationSize, unsigned int _GenerationLimit, unsigned int _ToursPerGen);

	bool IsTestComplete();
	
	std::string GetPopulationContentStr();
	std::string GetThresholdsStr(const std::shared_ptr<BlossomPlayer>& _Target);

private:
	unsigned int PopulationSize = 9;
	unsigned int GenerationLimit = 100;
	unsigned int ToursPerGen = 100;
	unsigned int TableSize = 9;
	
	unsigned int Generation = 0;
	unsigned int PlayersGenerated = 0;

	float MutateAmt = 0.375f;//0.25;

	unsigned int ParentLimit = 2;
	unsigned int TouramentSize = 4;
	unsigned int WinnerPerTouranment = 1;

	std::vector<std::shared_ptr<BlossomPlayer>> Population;
	std::vector<std::shared_ptr<Player>> PlayingPopulation;
	std::vector<std::shared_ptr<Participant>> RankingBoard;

	std::shared_ptr<Table> ActiveTable;
	std::vector<std::unique_ptr<Tournament>> Tournaments;
	std::vector<std::shared_ptr<Participant>> HoF;

	std::shared_ptr<Folder> FoldingPlayer;
	std::shared_ptr<Caller> CallingPlayer;
	std::shared_ptr<Raiser> RaisingPlayer;
	std::shared_ptr<Randomer> RandomPlayer;

	std::mt19937 MTGenerator;
	std::shared_ptr<HandEvaluator> Evaluator;
	std::unique_ptr<LogWriter> Writer;

	void GeneratePopulation(unsigned int _Size);

	void RankPlayer(const std::shared_ptr<BlossomPlayer>& _Player);
	void ArrangePlayers(std::vector<std::shared_ptr<BlossomPlayer>>& _Players);
	void ArrangeHoF();

	const std::shared_ptr<Participant>& GetParticipant(unsigned int _Index);
	const std::shared_ptr<Player>& GetBestPlayer();
	
	void TouramentSelect(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation, std::vector<std::shared_ptr<BlossomPlayer>>& _Parents);
	void Crossover(const std::shared_ptr<BlossomPlayer>& _First, const std::shared_ptr<BlossomPlayer>& _Second, std::shared_ptr<BlossomPlayer>& _Result);
	void Mutate(std::shared_ptr<BlossomPlayer>& _Target, Phase _Phase, unsigned int _ParaIndex);
	void ReproducePopulation();

	bool HasMutationHappen();

	float GetOverallFitness();
	float GetGenerationDiversity();
};

