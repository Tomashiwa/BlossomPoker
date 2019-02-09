#include "../inc/Precomputation.h"

Precomputation::Precomputation(const std::shared_ptr<HandEvaluator>& _Evaluator)
{
	Evaluator = _Evaluator;
}

Precomputation::~Precomputation()
{
}

void Precomputation::ComputePreflopOdds(unsigned int _OppoAmt, unsigned int _Trials)
{
	std::ofstream File("PreflopOdds.txt");

	std::array<Card,2> Hole { Card(Suit::Spade, Rank::Two), Card(Suit::Heart, Rank::Two) };
	std::vector<Card> Communal {};

	std::string HoleStr = "";
	float Odds = 0.0f;

	//Pocket Pairs
	std::cout << "Winning Odds w/ Pocket Pairs:\n";
	for (unsigned int RankIndex = 0; RankIndex < 13; RankIndex++)
	{
		Hole[0] = Card(Suit::Spade, (Rank)RankIndex);
		Hole[1] = Card(Suit::Heart, (Rank)RankIndex);

		HoleStr = Hole[0].To_String() + Hole[1].To_String();
		File << HoleStr << " ";

		for (unsigned int OppoIndex = 1; OppoIndex <= _OppoAmt; OppoIndex++)
		{
			Odds = Evaluator->DetermineOdds_MonteCarlo_Multi_OMPEval(Hole, Communal, OppoIndex, _Trials);

			std::cout << HoleStr << " against " << OppoIndex << ": " << Odds << "\n";
			File << Odds << " ";
		}

		File << "\n";
	}

	//Suited
	std::cout << "\nWinning Odds w/ Suited Holes:\n";
	std::vector<std::array<Card,2>> SuitedHoles;

	for (unsigned int FRankIndex = 0; FRankIndex < 13; FRankIndex++)
	{
		for (unsigned int SRankIndex = 0; SRankIndex < 13; SRankIndex++)
		{
			if (FRankIndex == SRankIndex)
				continue;

			Hole[0] = Card(Suit::Spade, (Rank)FRankIndex);
			Hole[1] = Card(Suit::Spade, (Rank)SRankIndex);

			HoleStr = Hole[0].To_String() + Hole[1].To_String();

			if (std::find_if(SuitedHoles.begin(), SuitedHoles.end(), [&](std::array<Card, 2> _Hole) { return (Hole[0].Get_Rank() == _Hole[0].Get_Rank() && Hole[1].Get_Rank() == _Hole[1].Get_Rank()) || (Hole[0].Get_Rank() == _Hole[1].Get_Rank() && Hole[1].Get_Rank() == _Hole[0].Get_Rank()); }) != SuitedHoles.end())
				continue;
			else
				SuitedHoles.push_back(Hole);

			File << HoleStr << " ";

			for (unsigned int OppoIndex = 1; OppoIndex <= _OppoAmt; OppoIndex++)
			{
				Odds = Evaluator->DetermineOdds_MonteCarlo_Multi_OMPEval(Hole, Communal, OppoIndex, _Trials);

				std::cout << HoleStr << " against " << OppoIndex << ": " << Odds << "\n";
				File << Odds << " ";
			}

			File << "\n";
		}
	}

	//Unsuited
	std::cout << "\nWinning Odds w/ Unsuited Holes:\n";
	std::vector<std::array<Card, 2>> UnsuitedHoles;

	for (unsigned int FRankIndex = 0; FRankIndex < 13; FRankIndex++)
	{
		for (unsigned int SRankIndex = 0; SRankIndex < 13; SRankIndex++)
		{
			if (FRankIndex == SRankIndex)
				continue;

			Hole[0] = Card(Suit::Spade, (Rank)FRankIndex);
			Hole[1] = Card(Suit::Heart, (Rank)SRankIndex);

			HoleStr = Hole[0].To_String() + Hole[1].To_String();

			if (std::find_if(UnsuitedHoles.begin(), UnsuitedHoles.end(), [&](std::array<Card, 2> _Hole) { return (Hole[0].Get_Rank() == _Hole[0].Get_Rank() && Hole[1].Get_Rank() == _Hole[1].Get_Rank()) || (Hole[0].Get_Rank() == _Hole[1].Get_Rank() && Hole[1].Get_Rank() == _Hole[0].Get_Rank()); }) != UnsuitedHoles.end())
				continue;
			else
				UnsuitedHoles.push_back(Hole);

			File << HoleStr << " ";

			for (unsigned int OppoIndex = 1; OppoIndex <= _OppoAmt; OppoIndex++)
			{
				Odds = Evaluator->DetermineOdds_MonteCarlo_Multi_OMPEval(Hole, Communal, OppoIndex, _Trials);
				std::cout << HoleStr << " against " << OppoIndex << ": " << Odds << "\n";
				File << Odds << " ";
			}

			File << "\n";
		}
	}

	File.close();
}
