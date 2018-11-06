#pragma once
#include <vector>
#include <chrono>
#include <random>
#include <memory>
#include <time.h>

#include "LogWriter.h"

class Table;
class Player;
class Tournament;

class GeneticTest
{
public:
	GeneticTest();
	~GeneticTest();

	void Start();
	void Run();
	void End();
	void Reset();

	void SetSpecs(unsigned int _PopulationSize, unsigned int _GenerationLimit);

	bool IsTestComplete();
	void PrintPopulationFitness();

	std::string GetPopulationContentStr();
	std::string GetThresholdsStr(const std::shared_ptr<Player>& _Target);

private:
	unsigned int PopulationSize = 9;
	unsigned int GenerationLimit = 100;
	float TargetFitness = 0.75;

	unsigned int Generation = 0;
	unsigned int PlayersGenerated = 0;

	unsigned int ParentLimit = 2;
	unsigned int TouramentSize = 2;
	unsigned int WinnerPerTouranment = 1;

	std::vector<std::shared_ptr<Player>> Population;

	std::unique_ptr<Tournament> Tour;
	std::shared_ptr<Player> CurrentBest;
	std::vector<std::shared_ptr<Player>> GeneratonBest;

	std::mt19937 MTGenerator;
	std::unique_ptr<LogWriter> Writer;

	void GeneratePopulation(unsigned int _Size);
	
	void TouramentSelect(const std::vector<std::shared_ptr<Player>> _RefPopulation, std::vector<std::shared_ptr<Player>>& _Parents);
	//void AlternisSelect(std::vector<PlayerEntry>&);
	void Crossover(const std::shared_ptr<Player>& _First, const std::shared_ptr<Player>& _Second, std::shared_ptr<Player>& _Result);
	void Mutate(std::shared_ptr<Player>& _Target, unsigned int _ParaIndex);
	void ReproducePopulation();

	bool HasMutationHappen();

	float GetOverallFitness();
	float GetGenerationDiversity();
};

