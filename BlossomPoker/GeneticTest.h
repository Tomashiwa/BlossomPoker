#pragma once
#include <array>
#include <vector>
#include <map>
#include <chrono>
#include <random>
#include <memory>
#include <time.h>

#include "LogWriter.h"

class Board;
class Player;
class HandEvaluator;

class GeneticTest
{
public:
	GeneticTest();
	~GeneticTest();

	void Start();
	void Update();
	void End();

	void Reset();

	void SetSpecs(unsigned int _PopulationSize, unsigned int _SubjectsSize, unsigned int _GenerationLimit);

	bool IsTestComplete();
	std::string GetPopulationStr();
	std::string GetThresholdsStr(Player& _Target);
	void PrintPopulationFitness();

private:
	struct PlayerEntry
	{
		std::shared_ptr<Player> TargetPlayer;
		double WinRate;

		PlayerEntry(std::shared_ptr<Player> _Target, double _WinRate) : TargetPlayer(_Target), WinRate(_WinRate) {}
		
		bool operator== (const PlayerEntry &e) 
		{
			return (TargetPlayer == e.TargetPlayer && WinRate == e.WinRate);
		}
	};

	double TargetFitness = 0.75;
	double MutationRate = 0.05;

	unsigned int PopulationSize = 8;
	unsigned int SubjectsAmt = 10;
	unsigned int TouramentSize = 2;
	unsigned int WinnerPerTouranment = 1;

	unsigned int ParentLimit = 2;
	unsigned int RoundLimit = 100;
	unsigned int GenerationLimit = 50;

	unsigned int Generation = 0;
	unsigned int PlayersGenerated = 0;

	double Mutate_LearningFac = 1.1;
	double Mutate_ExploreFac = 1.5;

	std::mt19937 MTGenerator;

	std::shared_ptr<Board> TestBoard;
	std::shared_ptr<HandEvaluator> Evaluator;
	std::unique_ptr<LogWriter> Writer;

	std::vector<PlayerEntry> Population;
	std::vector<std::shared_ptr<Player>> RandomSubjects;

	std::shared_ptr<PlayerEntry> FittestPlayer;
	std::vector<std::shared_ptr<PlayerEntry>> FittestInGenerations;

	void InitializePopulation(unsigned int _Size);
	void GenerateSubjects(unsigned int _Size);
	void MeasureFitness();
	
	void TouramentSelect(const std::vector<PlayerEntry>& _ReferencePop, std::vector<PlayerEntry>& _Parents);
	//void AlternisSelect(std::vector<PlayerEntry>&);
	void Crossover(const std::shared_ptr<Player>& _First, const std::shared_ptr<Player>& _Second, std::shared_ptr<Player>& _Result);
	void Mutate(std::shared_ptr<Player>& _Target, unsigned int _ParaIndex);

	double DetermineWinRate(std::shared_ptr<Player> _Current, std::shared_ptr<Player> _Subject);
	double GetOverallFitness();
	double GetGenerationDiversity();

	bool HasHigherFitness(std::pair<std::shared_ptr<Player>, double> _First, std::pair<std::shared_ptr<Player>, double> _Second);
	bool HasMutationHappen();
	void ReproducePopulation();
};

