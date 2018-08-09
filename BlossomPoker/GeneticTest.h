#pragma once
#include <array>
#include <vector>
#include <map>
#include <chrono>
#include <random>
#include <time.h>

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

	bool IsTestComplete();
	std::string GetPopulationStr();
	std::string GetThresholdsStr(Player* _Target);
	void PrintPopulationFitness();

private:
	void InitializePopulation(unsigned int _Size);
	void GenerateSubjects(unsigned int _Size);
	void MeasureFitness();
	
	double DetermineWinRate(Player* _Current, Player* _Subject);
	double GetOverallFitness();

	bool HasHigherFitness(std::pair<Player*, double> _First, std::pair<Player*, double> _Second);
	bool HasMutationHappen();
	void ReproducePopulation();

	Board* TestBoard;
	HandEvaluator* Evaluator;
	std::map<Player*, double> Population;
	std::vector<Player*> RandomSubjects;

	double TargetFitness = 0.85;
	double BestFitness = 0.0;

	double MutationRate = 0.05;

	unsigned int RoundLimit = 100;
	unsigned int TrialLimit = 1;
	unsigned int ParentLimit = 2;
	unsigned int TouranmentSize = 4;
	unsigned int WinnerPerTouranment = 1;
	unsigned int PlayersGenerated = 0;
	unsigned int Generation = 0;
};

