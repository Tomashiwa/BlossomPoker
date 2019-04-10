#pragma once
#include <vector>
#include <chrono>
#include <random>
#include <memory>
#include <time.h>

#include "../../Table/inc/Phase.h"
#include "../inc//TrainingModel.h"
#include "../inc/LogWriter.h"

#include "Selector.h"
#include "Crossoverer.h"
#include "Mutator.h"

class Table;
class Player;
class BlossomPlayer;
class Folder;
class Caller;
class Raiser;
class Randomer;
class CallRaiser;
class Tournament;
class HandEvaluator;
class Precomputation;

class GeneticTrainer
{
public:
	GeneticTrainer();
	~GeneticTrainer();

	void SetSpecs(TrainingModel _Model, Layer _Layer);

	void Initialize();
	void Run();
	void End();
	void Reset();

	bool IsTrainingCompleted();
	
private:
	TrainingModel Model;
	Layer FeedbackLayer;

	unsigned int TableSize = 9;
	
	unsigned int Generation = 0;
	unsigned int PlayersGenerated = 0;

	std::vector<std::shared_ptr<BlossomPlayer>> Population;
	std::vector<std::shared_ptr<BlossomPlayer>> EvaluatingPopulation;
	std::vector<std::shared_ptr<Player>> PlayingPopulation;

	std::shared_ptr<Table> ActiveTable;
	std::vector<std::unique_ptr<Tournament>> Tournaments;

	std::unique_ptr<Selector> ActiveSelector;
	std::unique_ptr<Crossoverer> ActiveCrossoverer;
	std::unique_ptr<Mutator> ActiveMutator;

	void InitializePopulation(unsigned int _Size);
	void InitializeEvaluatingPopu();
	void InitializePlayingPopu();

	float MeasureFitness(const std::shared_ptr<BlossomPlayer>& _Player);

	std::shared_ptr<BlossomPlayer> TournamentSelect_Fitness(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation);
	std::shared_ptr<BlossomPlayer> TournamentSelect_Uniqueness(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation);
	std::shared_ptr<BlossomPlayer> TournamentSelect_Potential(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation);

	void ReproducePopulation();

	void RankPlayers();

	float GetOverallFitness();
	float GetGenerationDiversity();

	//Elitism
	unsigned int ElitesLimit;
	
	//Hall-of-Fame
	unsigned int HoFSize;
	std::vector<std::shared_ptr<BlossomPlayer>> HoF;

	void AddToHoF(unsigned int _Amt);
	void ArrangeHoF();
	void ClipHoF(unsigned int _Size);

	//Population Culling & Nuking
	unsigned int CullCount = 0;
	std::vector<float> GenerationAverFitness;

	//Reserved Selection
	unsigned int ReserveSize;
	std::vector<unsigned int> SelectionTable;
	std::vector<std::shared_ptr<BlossomPlayer>> NRAPopulation;
	std::vector<std::shared_ptr<BlossomPlayer>> RAPopulation;

	std::shared_ptr<BlossomPlayer> Adapt(const std::shared_ptr<BlossomPlayer>& _Target, const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation);

	//Tools
	std::mt19937 MTGenerator;
	std::unique_ptr<LogWriter> Writer;
	std::shared_ptr<HandEvaluator> Evaluator;
	std::unique_ptr<Precomputation> Precomp;
	
	//Population Culling & Nuking
	bool HasPopulationStagnate();
	void CullPopulation();
	void NukePopulation();

	void PrintPlayerResult(std::shared_ptr<BlossomPlayer>& _Player);
	void PrintGenerationResult();

	std::string GetPopulationContentStr();
	std::string GetThresholdsStr(const std::shared_ptr<BlossomPlayer>& _Target);

	//Variables to track
	unsigned int MutatePhase = 0;
	std::shared_ptr<BlossomPlayer> WorstPlayer;
	std::shared_ptr<BlossomPlayer> BestPlayer;
};

