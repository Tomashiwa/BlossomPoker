#pragma once
#include <vector>
#include <chrono>
#include <random>
#include <memory>
#include <time.h>

#include "../../Table/inc/Phase.h"
#include "../inc//TrainingModel.h"
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

	unsigned int TableSize = 8;
	
	unsigned int Generation = 0;
	unsigned int PlayersGenerated = 0;

	std::vector<std::shared_ptr<BlossomPlayer>> Population;
	std::vector<std::shared_ptr<Player>> PlayingPopulation;
	std::vector<std::shared_ptr<Participant>> RankingBoard;

	std::shared_ptr<Table> ActiveTable;
	std::vector<std::unique_ptr<Tournament>> Tournaments;

	void InitializePopulation(unsigned int _Size);
	void InitializePlayingPopu();

	float MeasureFitness(const std::shared_ptr<BlossomPlayer>& _Player);

	std::shared_ptr<BlossomPlayer> TournamentSelect_Fitness(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation);
	std::shared_ptr<BlossomPlayer> TournamentSelect_Uniqueness(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation);
	std::shared_ptr<BlossomPlayer> TournamentSelect_Potential(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation);

	void ReproducePopulation();

	bool HasCrossoverHappen();
	void Crossover(const std::shared_ptr<BlossomPlayer>& _First, const std::shared_ptr<BlossomPlayer>& _Second, std::vector<std::shared_ptr<BlossomPlayer>>& _Results);
	
	void EvaluateMutateRate();
	bool HasMutationHappen();
	bool Mutate(std::shared_ptr<BlossomPlayer>& _Target);// , Phase _Phase);

	void RankPlayer(const std::shared_ptr<BlossomPlayer>& _Player);
	void ArrangePlayers(std::vector<std::shared_ptr<BlossomPlayer>>& _Players);

	float GetOverallFitness();
	float GetGenerationDiversity();

	//Elitism
	unsigned int ElitesLimit;
	
	//Hall-of-Fame
	unsigned int HoFSize;
	std::vector<std::shared_ptr<Participant>> HoF;

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

	float MeasureUniqueness(const std::shared_ptr<BlossomPlayer>& _Player);
	float MeasurePotential(const std::shared_ptr<BlossomPlayer>& _Player);
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
	const std::shared_ptr<Participant>& GetParticipant(unsigned int _Index);

	//Variables to track
	float BestFitness = 0.0f;
	unsigned int MutatePhase = 0;
	std::shared_ptr<BlossomPlayer> BestPlayer;
};

