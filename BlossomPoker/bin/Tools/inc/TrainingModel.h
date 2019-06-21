#pragma once

enum Selection { Tour, Truncation, Roulette, StochasticSampling};
enum Crossover { KPoint, Uniform, Blending };
enum Mutation  { GaussianOffset };

enum Scaling { Static, Gaussian};

enum Layer { Generational, Individual };

struct TrainingModel
{
	Layer FeedbackLayer = Layer::Generational;

	unsigned int PopulationSize = 9;
	unsigned int GenerationLimit = 100;
	unsigned int TournamentsPerGen = 9;

	Selection SelectMethod = Selection::Tour;
	unsigned int ParentsAmt = 2;
	unsigned int TournamentSize = 2;

	Selection SelectChildMethod = Selection::Truncation;

	Crossover CrossMethod = Crossover::KPoint;
	unsigned int KPointCount = 2;
	float CrossoverRate = 0.5f;
	Scaling CrossoverScale = Scaling::Static;

	Mutation MutateMethod = Mutation::GaussianOffset;
	float GaussianOffset = 0.25f;
	float MutationRate = 0.01f;
	Scaling MutationScale = Scaling::Static;

	bool IsOverlapping = false;
	float ChildPopulationRatio = 1.0f;

	bool HasElite = false;
	float EliteRatio = 0.125f;

	bool HasHoF = false;
	float HoFRatio = 1.0f;

	bool HasCulling = false;
	bool HasNuking = false;
	float MinimumFitnessDiff = 5.0f;
	unsigned int StagnateInterval = 200;
	unsigned int StagnatePeriod = 200;
	unsigned int MaxCullCount = 10;

	bool HasReserveSelection = false;
	float AdaptationRate = 0.25f;
	float ReserveRatio = 0.125f;
	unsigned int SamplingBreadth = 5;
};