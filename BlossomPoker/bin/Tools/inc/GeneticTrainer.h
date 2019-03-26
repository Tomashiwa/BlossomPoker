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

	void Start();
	void Run();
	void End();
	void Reset();

	void SetSpecs(TrainingModel _Model, Layer _Layer);// unsigned int _PopulationSize, unsigned int _GenerationLimit, unsigned int _ToursPerGen);

	bool IsTestComplete();
	
	std::string GetPopulationContentStr();
	std::string GetThresholdsStr(const std::shared_ptr<BlossomPlayer>& _Target);

private:
	TrainingModel Model;
	Layer FeedbackLayer;

	unsigned int TableSize = 8;
	
	unsigned int Generation = 0;
	unsigned int PlayersGenerated = 0;

	unsigned int ElitesLimit;

	unsigned int MutatePhase = 0;

	unsigned int ReserveSize;

	unsigned int CullCount = 0;

	std::vector<std::shared_ptr<BlossomPlayer>> Population;
	std::vector<std::shared_ptr<BlossomPlayer>> NRAPopulation;
	std::vector<std::shared_ptr<BlossomPlayer>> RAPopulation;

	std::vector<unsigned int> SelectionTable;
	std::vector<float> GenerationAverFitness;

	std::vector<std::shared_ptr<Player>> PlayingPopulation;
	std::vector<std::shared_ptr<Participant>> RankingBoard;

	std::shared_ptr<Table> ActiveTable;
	std::vector<std::unique_ptr<Tournament>> Tournaments;
	std::vector<std::shared_ptr<Participant>> HoF;

	std::shared_ptr<Caller> CallingPlayer;
	std::shared_ptr<Raiser> RaisingPlayer;
	std::shared_ptr<Randomer> RandomPlayer0;
	std::shared_ptr<Randomer> RandomPlayer1;

	std::shared_ptr<BlossomPlayer> BestPlayer;

	std::mt19937 MTGenerator;
	std::shared_ptr<HandEvaluator> Evaluator;
	std::unique_ptr<LogWriter> Writer;

	void InitializePopulation(unsigned int _Size);

	void InitializePlayingPopu();

	float MeasureFitness(const std::shared_ptr<BlossomPlayer>& _Player);
	float MeasureUniqueness(const std::shared_ptr<BlossomPlayer>& _Player);
	float MeasurePotential(const std::shared_ptr<BlossomPlayer>& _Player);

	void RankPlayer(const std::shared_ptr<BlossomPlayer>& _Player);
	void ArrangePlayers(std::vector<std::shared_ptr<BlossomPlayer>>& _Players);

	void AddPlayersToHoF(unsigned int _Amt);
	void ArrangeHoF();
	void ClipHoF(unsigned int _Size);

	const std::shared_ptr<Participant>& GetParticipant(unsigned int _Index);
	
	std::shared_ptr<BlossomPlayer> TournamentSelect_Fitness(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation);
	std::shared_ptr<BlossomPlayer> TournamentSelect_Uniqueness(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation);
	std::shared_ptr<BlossomPlayer> TournamentSelect_Potential(const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation);

	void Crossover(const std::shared_ptr<BlossomPlayer>& _First, const std::shared_ptr<BlossomPlayer>& _Second, std::vector<std::shared_ptr<BlossomPlayer>>& _Results);
	bool Mutate(std::shared_ptr<BlossomPlayer>& _Target);// , Phase _Phase);
	std::shared_ptr<BlossomPlayer> Adapt(const std::shared_ptr<BlossomPlayer>& _Target, const std::vector<std::shared_ptr<BlossomPlayer>> _RefPopulation);
	
	void ReproducePopulation();

	void CullPopulation();
	void NukePopulation();

	bool HasPopulationStagnate();

	bool HasCrossoverHappen();

	void EvaluateMutateRate();
	bool HasMutationHappen();

	float GetOverallFitness();
	float GetGenerationDiversity();

	std::unique_ptr<Precomputation> Precomp;
};

