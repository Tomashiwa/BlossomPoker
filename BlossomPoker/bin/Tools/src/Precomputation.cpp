#include "../inc/Precomputation.h"

Precomputation::Precomputation(const std::shared_ptr<HandEvaluator>& _Evaluator)
{
	Evaluator = _Evaluator;

	int ReferenceIndex = 0;
	ReferenceDeck.reserve(52);

	for (unsigned int SIndex = 0; SIndex < 4; SIndex++)
	{
		for (unsigned int RIndex = 0; RIndex < 13; RIndex++)
		{
			ReferenceDeck.emplace_back(static_cast<Suit>(SIndex), static_cast<Rank>(RIndex));
			ReferenceIndex++;
		}
	}
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

void Precomputation::ComputeFlopOdds(unsigned int _OppoAmt, unsigned int _Trials)
{
	std::ofstream File("FlopOdds.txt");

	std::vector<Card> Hand;
	std::vector<std::vector<Card>> Hands;

	std::string HandStr;

	/*//Royal Flush

	//Hole: Suited from 10 ~ A
	//Community: Suited filling in the gap of 10~A straight
	//Suit Restriction for Hole & Community: Spade
	//Mask: Any suit will be Spade
	for (unsigned int Hole_0 = 8; Hole_0 <= 11; Hole_0++)
	{
		Hand.emplace_back(Suit::Spade, static_cast<Rank>(Hole_0));

		for (unsigned int Hole_1 = Hole_0 + 1; Hole_1 <= 12; Hole_1++)
		{
			Hand.emplace_back(Suit::Spade, static_cast<Rank>(Hole_1));
			FillStraightGap(Hand, Rank::Ten, Rank::Ace);

			if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
			{
				Hands.push_back(Hand);

				std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
				ComputeOdd(Hand, _OppoAmt, _Trials, File);
			}

			Hand.erase(Hand.begin() + 1, Hand.end());
		}

		Hand.clear();
	}

	Hands.clear();

	//Straight Flush

	//Hole cards: Suited with rank difference range of 4
	//Community: Suited filling in the gap of the different straights
	//Suit Restriction for Hole & Community: Spade
	//Mask: Any suit will be Spade
	for (int Hole_0 = 0; Hole_0 <= 11; Hole_0++)
	{
		Hand.emplace_back(Suit::Spade, static_cast<Rank>(Hole_0));

		for (int Hole_1 = Hole_0 + 1; Hole_1 <= 12; Hole_1++)
		{
			unsigned int RankDiff = std::abs(Hole_1 - Hole_0);
			if (RankDiff > 4)
				continue;

			Hand.emplace_back(Suit::Spade, static_cast<Rank>(Hole_1));

			if (RankDiff == 4)
			{
				//3 center ie. 5 6 7 8 9
				FillStraightGap(Hand, Hand[0].Get_Rank(), Hand[1].Get_Rank());

				if (IsStraightValid(Hand) && std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
				{
					Hands.push_back(Hand);
					std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
					ComputeOdd(Hand, _OppoAmt, _Trials, File);
				}
			}
			else if (RankDiff == 3)
			{
				FillStraightGap(Hand, Hand[0].Get_Rank(), Hand[1].Get_Rank());

				//2 center, 1 front ie. 5 6 7 8 9
				if (Hand[0].Get_Rank() != Rank::Two)
				{
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[0].Get_Rank() - 1));

					if (IsStraightValid(Hand) && std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
					{
						Hands.push_back(Hand);
						std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
						ComputeOdd(Hand, _OppoAmt, _Trials, File);
					}

					Hand.pop_back();
				}

				//2 center, 1 back ie. 6 7 8 9 10
				if (Hand[1].Get_Rank() != Rank::Ace)
				{
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[1].Get_Rank() + 1));

					if (IsStraightValid(Hand) && std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
					{
						Hands.push_back(Hand);
						std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
						ComputeOdd(Hand, _OppoAmt, _Trials, File);
					}

					Hand.pop_back();
				}
			}
			else if (RankDiff == 2)
			{
				FillStraightGap(Hand, Hand[0].Get_Rank(), Hand[1].Get_Rank());

				//1 center, 1 front, 1 back ie. 6 7 8 9 10
				if (Hand[0].Get_Rank() != Rank::Two && Hand[1].Get_Rank() != Rank::Ace)
				{
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[0].Get_Rank() - 1));
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[1].Get_Rank() + 1));

					if (IsStraightValid(Hand) && std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
					{
						Hands.push_back(Hand);
						std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
						ComputeOdd(Hand, _OppoAmt, _Trials, File);
					}

					Hand.pop_back();
					Hand.pop_back();
				}
			
				//1 center, 2 front ie. 5 6 7 8 9
				if ((int)Hand[0].Get_Rank() > (int)Rank::Three)
				{
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[0].Get_Rank() - 1));
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[0].Get_Rank() - 2));

					if (IsStraightValid(Hand) && std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
					{
						Hands.push_back(Hand);
						std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
						ComputeOdd(Hand, _OppoAmt, _Trials, File);
					}

					Hand.pop_back();
					Hand.pop_back();
				}

				//1 center, 2 back ie. 7 8 9 10 J
				if ((int)Hand[1].Get_Rank() < (int)Rank::King)
				{
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[1].Get_Rank() + 1));
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[1].Get_Rank() + 2));

					if (IsStraightValid(Hand) && std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
					{
						Hands.push_back(Hand);
						std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
						ComputeOdd(Hand, _OppoAmt, _Trials, File);
					}

					Hand.pop_back();
					Hand.pop_back();
				}
			}
			else if (RankDiff == 1)
			{
				//3 Cards at the back ie. 8 9 10 J Q
				if ((int)Hand[1].Get_Rank() < (int)Rank::Queen)
				{
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[1].Get_Rank() + 1));
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[1].Get_Rank() + 2));
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[1].Get_Rank() + 3));

					if (IsStraightValid(Hand) && std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
					{
						Hands.push_back(Hand);
						std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
						ComputeOdd(Hand, _OppoAmt, _Trials, File);
					}

					Hand.pop_back();
					Hand.pop_back();
					Hand.pop_back();
				}

				//2 Cards at back & 1 Card at front ie. 7 8 9 10 J
				if ((int)Hand[1].Get_Rank() < (int)Rank::King && Hand[0].Get_Rank() != Rank::Two)
				{
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[1].Get_Rank() + 1));
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[1].Get_Rank() + 2));

					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[0].Get_Rank() - 1));

					if (IsStraightValid(Hand) && std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
					{
						Hands.push_back(Hand);
						std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
						ComputeOdd(Hand, _OppoAmt, _Trials, File);
					}

					Hand.pop_back();
					Hand.pop_back();
					Hand.pop_back();
				}

				//1 Card at back & 2 Cards at front ie. 6 7 8 9 10
				if (Hand[1].Get_Rank() != Rank::Ace && (int)Hand[0].Get_Rank() > (int)Rank::Three)
				{
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[1].Get_Rank() + 1));

					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[0].Get_Rank() - 1));
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[0].Get_Rank() - 2));

					if (IsStraightValid(Hand) && std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
					{
						Hands.push_back(Hand);
						std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
						ComputeOdd(Hand, _OppoAmt, _Trials, File);
					}

					Hand.pop_back();
					Hand.pop_back();
					Hand.pop_back();
				}

				//3 Cards at front ie. 5 6 7 8 9
				if ((int)Hand[0].Get_Rank() > (int)Rank::Four)
				{
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[0].Get_Rank() - 1));
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[0].Get_Rank() - 2));
					Hand.emplace_back(Suit::Spade, static_cast<Rank>((int)Hand[0].Get_Rank() - 3));

					if (IsStraightValid(Hand) && std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
					{
						Hands.push_back(Hand);
						std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
						ComputeOdd(Hand, _OppoAmt, _Trials, File);
					}

					Hand.pop_back();
					Hand.pop_back();
					Hand.pop_back();
				}

			}

			Hand.erase(Hand.begin() + 1, Hand.end());
		}

		Hand.clear();
	}

	Hands.clear();

	//4-of-a-kind

	//Hole Cards: Pocket Pair
	//Community: Pair of same rank + 1 random card
	//Suit Restriction: Suit/Heart/Club/Diamond (4-of-a-kind) Any Suit (Kicker)
	//Mask: Suits for cards involved in the 4-of-a-kind to the approriate suit
	for (unsigned int FourKindIndex = 0; FourKindIndex < 13; FourKindIndex++)
	{
		Hand.emplace_back(Suit::Spade, static_cast<Rank>(FourKindIndex));
		Hand.emplace_back(Suit::Heart, static_cast<Rank>(FourKindIndex));
		Hand.emplace_back(Suit::Club, static_cast<Rank>(FourKindIndex));
		Hand.emplace_back(Suit::Diamond, static_cast<Rank>(FourKindIndex));

		for (unsigned int SuitIndex = 0; SuitIndex < 4; SuitIndex++)
		{
			for (unsigned int RankIndex = 0; RankIndex < 13; RankIndex++)
			{
				if (RankIndex == FourKindIndex)
					continue;

				Hand.emplace_back(static_cast<Suit>(SuitIndex), static_cast<Rank>(RankIndex));

				if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
				{
					Hands.push_back(Hand);
					std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
					ComputeOdd(Hand, _OppoAmt, _Trials, File);
				}

				Hand.pop_back();
			}
		}

		Hand.clear();
	}

	Hands.clear();

	//Hole Cards: 2 suited/unsuited cards that differ in rank
	//Community: 3-of-a-kind of same rank as one of the hole cards
	//Suit Restriction for Hole: Spade (1 out of 4-of-a-kind) Spade (Kicker)
	//Suit Restriction for Community: Heart / Club / Diamond(3 out of 4 - of - a - kind)
	//Mask: 1 from the 4-of-a-kind within the hole to be Spade and other 3 be Heart, Club and Diamond respectively, Kicker will be Spade
	for (unsigned int Hole_0 = 0; Hole_0 < 13; Hole_0++)
	{
		Hand.emplace_back(Suit::Spade, static_cast<Rank>(Hole_0));

		for (unsigned int Hole_1 = 0; Hole_1 < 13; Hole_1++)
		{
			if (Hole_0 == Hole_1)
				continue;

			Hand.emplace_back(Suit::Spade, static_cast<Rank>(Hole_1));

			Hand.emplace_back(Suit::Heart, static_cast<Rank>(Hole_0));
			Hand.emplace_back(Suit::Club, static_cast<Rank>(Hole_0));
			Hand.emplace_back(Suit::Diamond, static_cast<Rank>(Hole_0));

			if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
			{
				Hands.push_back(Hand);
				std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
				ComputeOdd(Hand, _OppoAmt, _Trials, File);
			}

			Hand.erase(Hand.begin() + 1, Hand.end());
		}

		Hand.clear();
	}
	
	Hands.clear();

	//Full House

	//Hole Cards: Pocket Pair
	//Community: 3-of-a-kind of a rank differing from the Pair
	//Suit Restriction for Hole: Spade/Heart
	//Suit Restriction for Community: Spade/Heart/Club
	//Mask: Pocket Pair to be Spade/Heart and the 3 cards to be Spade/Heart/Club
	for (unsigned int PairRank = 0; PairRank < 13; PairRank++)
	{
		Hand.emplace_back(Suit::Spade, static_cast<Rank>(PairRank));
		Hand.emplace_back(Suit::Heart, static_cast<Rank>(PairRank));

		for (unsigned int ThreeKindRank = 0; ThreeKindRank < 13; ThreeKindRank++)
		{
			if (PairRank == ThreeKindRank)
				continue;
		
			Hand.emplace_back(Suit::Spade, static_cast<Rank>(ThreeKindRank));
			Hand.emplace_back(Suit::Heart, static_cast<Rank>(ThreeKindRank));
			Hand.emplace_back(Suit::Club, static_cast<Rank>(ThreeKindRank));

			if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
			{
				Hands.push_back(Hand);
				std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
				ComputeOdd(Hand, _OppoAmt, _Trials, File);
			}

			Hand.erase(Hand.begin() + 2, Hand.end());
		}

		Hand.clear();
	}

	Hands.clear();

	//Hole Cards: 2 Suited/Unsuited of different rank
	//Community: 2 cards w/ same rank as 1 card within Hole and 1 card w/ same rank as the other card in Hole
	//Suit restriction for Hole: Spade (Suited Hole Cards), Spade/Heart (Unsuited Hole Cards)
	//Suit Restriction for community: Heart / Club(2 of 3 - of - a - kind) Heart / Spade(3rd community card)
	//Mask: Hole card to be Spade (Suited) or Spade/Heart (Unsuited), Community will be Heart/Club to fill up 3-of-a-kind and either Heart/Spade the 1 out of the pair within community
	for (unsigned int ThreeKindRank = 0; ThreeKindRank < 13; ThreeKindRank++)
	{
		Hand.emplace_back(Suit::Spade, static_cast<Rank>(ThreeKindRank));

		for (unsigned int PairRank = 0; PairRank < 13; PairRank++)
		{
			if (ThreeKindRank == PairRank)
				continue;

			//Suited Hole
			Hand.emplace_back(Suit::Spade, static_cast<Rank>(PairRank));
			Hand.emplace_back(Suit::Heart, static_cast<Rank>(ThreeKindRank));
			Hand.emplace_back(Suit::Club, static_cast<Rank>(ThreeKindRank));
			Hand.emplace_back(Suit::Heart, static_cast<Rank>(PairRank));

			if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
			{
				Hands.push_back(Hand);
				std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
				ComputeOdd(Hand, _OppoAmt, _Trials, File);
			}

			Hand.erase(Hand.begin() + 1, Hand.end());

			//Unsuited Hole
			Hand.emplace_back(Suit::Heart, static_cast<Rank>(PairRank));
			Hand.emplace_back(Suit::Heart, static_cast<Rank>(ThreeKindRank));
			Hand.emplace_back(Suit::Club, static_cast<Rank>(ThreeKindRank));
			Hand.emplace_back(Suit::Spade, static_cast<Rank>(PairRank));
			
			if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreCardsIdentical_Order(Hand, _ComparedTo); }) == Hands.end())
			{
				Hands.push_back(Hand);
				std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
				ComputeOdd(Hand, _OppoAmt, _Trials, File);
			}

			Hand.erase(Hand.begin() + 1, Hand.end());
		}

		Hand.clear();
	}

	Hands.clear();

	//Flush
	//Hole Cards: 2 suited cards
	//Community: 3 cards w/ same suit as hole cards
	//Suit restriction for Hole & Community: Spade
	//Mask: If Hole & Community all share the same suit, convert to Spade
	for (unsigned int Hole_0 = 0; Hole_0 < 13; Hole_0++)
	{
		Hand.emplace_back(Suit::Spade, static_cast<Rank>(Hole_0));

		for (unsigned int Hole_1 = 0; Hole_1 < 13; Hole_1++)
		{
			if (std::find_if(Hand.begin(), Hand.end(), [&](Card ComparedTo) { return Hole_1 == (unsigned int)ComparedTo.Get_Rank(); }) != Hand.end())
				continue;

			Hand.emplace_back(Suit::Spade, static_cast<Rank>(Hole_1));

			for (unsigned int Hole_2 = 0; Hole_2 < 13; Hole_2++)
			{
				if (std::find_if(Hand.begin(), Hand.end(), [&](Card ComparedTo) { return Hole_2 == (unsigned int)ComparedTo.Get_Rank(); }) != Hand.end())
					continue;

				Hand.emplace_back(Suit::Spade, static_cast<Rank>(Hole_2));

				for (unsigned int Hole_3 = 0; Hole_3 < 13; Hole_3++)
				{
					if (std::find_if(Hand.begin(), Hand.end(), [&](Card ComparedTo) { return Hole_3 == (unsigned int)ComparedTo.Get_Rank(); }) != Hand.end())
						continue;

					Hand.emplace_back(Suit::Spade, static_cast<Rank>(Hole_3));

					for (unsigned int Hole_4 = 0; Hole_4 < 13; Hole_4++)
					{
						if (std::find_if(Hand.begin(), Hand.end(), [&](Card ComparedTo) { return Hole_4 == (unsigned int)ComparedTo.Get_Rank(); }) != Hand.end())
							continue;

						Hand.emplace_back(Suit::Spade, static_cast<Rank>(Hole_4));

						if (!IsStraightValid(Hand) && std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreHandsIdentical(Hand, _ComparedTo); }) == Hands.end())
						{
							Hands.push_back(Hand);
							std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
							ComputeOdd(Hand, _OppoAmt, _Trials, File);
						}

						Hand.erase(Hand.begin() + 4, Hand.end());
					}

					Hand.erase(Hand.begin() + 3, Hand.end());
				}

				Hand.erase(Hand.begin() + 2, Hand.end());
			}

			Hand.erase(Hand.begin() + 1, Hand.end());
		}

		Hand.clear();
	}

	Hands.clear();

	//Straight
	//Hole Cards: cards with rank difference of 4
	//Community: 3 cards that filled the gap in straight
	//Suit restriction for Hole & Community; At least 1 odds suit must exist within the hand
	//Mask: -
	for (unsigned int Hole_0 = 0; Hole_0 <= ReferenceDeck.size() - 1; Hole_0++)
	{
		Hand.push_back(ReferenceDeck[Hole_0]);

		for (unsigned int Hole_1 = Hole_0 + 1; Hole_1 < ReferenceDeck.size(); Hole_1++)
		{
			if (!IsCardInStraightRange(ReferenceDeck[Hole_1], Hand))
				continue;

			Hand.push_back(ReferenceDeck[Hole_1]);

			for (unsigned int Comm_0 = 0; Comm_0 < ReferenceDeck.size() - 2; Comm_0++)
			{
				if (IsCardInHand(ReferenceDeck[Comm_0], Hand) || !IsCardInStraightRange(ReferenceDeck[Comm_0], Hand))
					continue;

				for (auto const& Card : Hand)
				{
					if (std::abs((int)ReferenceDeck[Comm_0].Get_Rank() - (int)Card.Get_Rank()))
						continue;
				}

				Hand.push_back(ReferenceDeck[Comm_0]);

				for (unsigned int Comm_1 = Comm_0 + 1; Comm_1 < ReferenceDeck.size() - 1; Comm_1++)
				{
					if (IsCardInHand(ReferenceDeck[Comm_1], Hand) || !IsCardInStraightRange(ReferenceDeck[Comm_1], Hand))
						continue;
				
					Hand.push_back(ReferenceDeck[Comm_1]);

					for (unsigned int Comm_2 = Comm_1 + 1; Comm_2 < ReferenceDeck.size(); Comm_2++)
					{
						if (IsCardInHand(ReferenceDeck[Comm_2], Hand) || !IsCardInStraightRange(ReferenceDeck[Comm_2], Hand))
							continue;

						Hand.push_back(ReferenceDeck[Comm_2]);

						if (!IsFlushValid(Hand) && IsStraightValid(Hand) && std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreHandsIdentical(Hand, _ComparedTo); }) == Hands.end())
						{
							Hands.push_back(Hand);
							std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
							ComputeOdd(Hand, _OppoAmt, _Trials, File);
						}

						Hand.pop_back();
					}

					Hand.pop_back();
				}

				Hand.pop_back();
			}

			Hand.pop_back();
		}

		Hand.clear();
	}

	Hands.clear();

	//3-of-a-kind
	//Hole Cards: Pocket Pair
	//Community: 1 card sharing same rank as Pocket Pair & 2 random cards with differing rank
	//Suit restriction of Hole & Community: -
	//Mask: -
	for (unsigned int ThreeKind_Rank = 0; ThreeKind_Rank < 13; ThreeKind_Rank++)
	{
		for (unsigned int ThreeKind_Suit_0 = 0; ThreeKind_Suit_0 < 3; ThreeKind_Suit_0++)
		{
			Hand.emplace_back(static_cast<Suit>(ThreeKind_Suit_0), static_cast<Rank>(ThreeKind_Rank));
		
			for (unsigned int ThreeKind_Suit_1 = ThreeKind_Suit_0 + 1; ThreeKind_Suit_1 < 4; ThreeKind_Suit_1++)
			{
				if (IsCardInHand(static_cast<Rank>(ThreeKind_Rank), static_cast<Suit>(ThreeKind_Suit_1), Hand))
					continue;

				Hand.emplace_back(static_cast<Suit>(ThreeKind_Suit_1), static_cast<Rank>(ThreeKind_Rank));
			
				for (unsigned int ThreeKind_Suit_2 = 0; ThreeKind_Suit_2 < 4; ThreeKind_Suit_2++)
				{
					if (IsCardInHand(static_cast<Rank>(ThreeKind_Rank), static_cast<Suit>(ThreeKind_Suit_2), Hand))
						continue;
					
					Hand.emplace_back(static_cast<Suit>(ThreeKind_Suit_2), static_cast<Rank>(ThreeKind_Rank));

					for (unsigned int Comm_0 = 0; Comm_0 < ReferenceDeck.size() - 1; Comm_0++)
					{
						if (IsCardInHand(ReferenceDeck[Comm_0], Hand))
							continue;

						Hand.push_back(ReferenceDeck[Comm_0]);

						for (unsigned int Comm_1 = Comm_0 + 1; Comm_1 < ReferenceDeck.size(); Comm_1++)
						{
							if (ReferenceDeck[Comm_1].Get_Rank() == ReferenceDeck[Comm_0].Get_Rank()|| IsCardInHand(ReferenceDeck[Comm_1], Hand))
								continue;

							Hand.push_back(ReferenceDeck[Comm_1]);

							if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreHandsIdentical(Hand, _ComparedTo); }) == Hands.end())
							{
								Hands.push_back(Hand);
								std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
								ComputeOdd(Hand, _OppoAmt, _Trials, File);
							}

							Hand.pop_back();
						}

						Hand.pop_back();
					}

					Hand.pop_back();
				}

				Hand.pop_back();
			}

			Hand.clear();
		}
	}

	Hands.clear();

	//Hole Cards: 1 card of 3-of-a-kind & 1 random card of differing rank
	//Community: 2 cards of 3-of-a-kind & 1 random card of differing rank
	//Suit restriction of Hole & Community: -
	//Mask: -
	for (unsigned int ThreeKind_Rank = 0; ThreeKind_Rank < 13; ThreeKind_Rank++)
	{
		for (unsigned int Hole_0 = 0; Hole_0 < 4; Hole_0++)
		{
			Hand.emplace_back(static_cast<Suit>(Hole_0), static_cast<Rank>(ThreeKind_Rank));

			for (unsigned int Hole_1 = 0; Hole_1 < ReferenceDeck.size(); Hole_1++)
			{
				if (IsCardInHand(ReferenceDeck[Hole_1], Hand) || (unsigned int) ReferenceDeck[Hole_1].Get_Rank() == ThreeKind_Rank)
					continue;

				Hand.push_back(ReferenceDeck[Hole_1]);

				for (unsigned int Comm_0 = 0; Comm_0 < 3; Comm_0++)
				{
					if (IsCardInHand(static_cast<Rank>(ThreeKind_Rank), static_cast<Suit>(Comm_0), Hand))
						continue;

					Hand.emplace_back(static_cast<Suit>(Comm_0), static_cast<Rank>(ThreeKind_Rank));

					for (unsigned int Comm_1 = Comm_0 + 1; Comm_1 < 4; Comm_1++)
					{
						if (IsCardInHand(static_cast<Rank>(ThreeKind_Rank), static_cast<Suit>(Comm_1), Hand))
							continue;

						Hand.emplace_back(static_cast<Suit>(Comm_1), static_cast<Rank>(ThreeKind_Rank));

						for (unsigned int Comm_2 = 0; Comm_2 < ReferenceDeck.size(); Comm_2++)
						{
							if (IsCardInHand(ReferenceDeck[Comm_2], Hand) || ReferenceDeck[Comm_2].Get_Rank() == ReferenceDeck[Hole_1].Get_Rank() || (unsigned int) ReferenceDeck[Comm_2].Get_Rank() == ThreeKind_Rank)
								continue;

							Hand.push_back(ReferenceDeck[Comm_2]);

							if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreHandsIdentical(Hand, _ComparedTo); }) == Hands.end())
							{
								Hands.push_back(Hand);
								std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
								ComputeOdd(Hand, _OppoAmt, _Trials, File);
							}

							Hand.pop_back();
						}

						Hand.pop_back();
					}

					Hand.pop_back();
				}

				Hand.pop_back();
			}

			Hand.clear();
		}
	}

	Hands.clear();

	//Hole Cards: 2 random cards of differing rank
	//Community: 3 cards of same rank representating 3-of-a-kind
	//Suit restriction for Hole & Community: Spade/Heart/Club (3-of-a-kind)
	//Mask: Any 3-of-a-kind will be Spade/Heart/Club
	for (unsigned int Hole_0 = 0; Hole_0 < ReferenceDeck.size() - 1; Hole_0++)
	{
		Hand.push_back(ReferenceDeck[Hole_0]);

		for (unsigned int Hole_1 = Hole_0 + 1; Hole_1 < ReferenceDeck.size(); Hole_1++)
		{
			if (IsCardInHand(ReferenceDeck[Hole_1], Hand))
				continue;

			Hand.push_back(ReferenceDeck[Hole_1]);

			for (unsigned int ThreeKind_Rank = 0; ThreeKind_Rank < 13; ThreeKind_Rank++)
			{
				if (ReferenceDeck[Hole_0].Get_Rank() == static_cast<Rank>(ThreeKind_Rank) || ReferenceDeck[Hole_1].Get_Rank() == static_cast<Rank>(ThreeKind_Rank))
					continue;

				Hand.emplace_back(Suit::Spade, static_cast<Rank>(ThreeKind_Rank));
				Hand.emplace_back(Suit::Heart, static_cast<Rank>(ThreeKind_Rank));
				Hand.emplace_back(Suit::Club, static_cast<Rank>(ThreeKind_Rank));

				if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreHandsIdentical(Hand, _ComparedTo); }) == Hands.end())
				{
					Hands.push_back(Hand);
					std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
					ComputeOdd(Hand, _OppoAmt, _Trials, File);
				}

				Hand.pop_back();
				Hand.pop_back();
				Hand.pop_back();
			}
			
			Hand.pop_back();
		}

		Hand.clear();
	}

	Hands.clear();

	//Two Pair
	//Hole Cards: Pocket Pair
	//Community: Pair of differing Rank from Hole Cards & 1 random card
	//Suit Restriction for Hole & Community: -
	//Mask: -
	for (unsigned int Pair_0 = 0; Pair_0 < 13; Pair_0++)
	{
		for (unsigned int Hole_0 = 0; Hole_0 < 3; Hole_0++)
		{
			Hand.emplace_back(static_cast<Suit>(Hole_0), static_cast<Rank>(Pair_0));

			for (unsigned int Hole_1 = Hole_0 + 1; Hole_1 < 4; Hole_1++)
			{
				if (IsCardInHand(static_cast<Rank>(Pair_0), static_cast<Suit>(Hole_1), Hand))
					continue;

				Hand.emplace_back(static_cast<Suit>(Hole_1), static_cast<Rank>(Pair_0));

				for (unsigned int Pair_1 = 0; Pair_1 < 13; Pair_1++)
				{
					if (Pair_1 == Pair_0)
						continue;

					for (unsigned int Comm_0 = 0; Comm_0 < 3; Comm_0++)
					{
						Hand.emplace_back(static_cast<Suit>(Comm_0), static_cast<Rank>(Pair_1));

						for (unsigned int Comm_1 = Comm_0 + 1; Comm_1 < 4; Comm_1++)
						{
							if (IsCardInHand(static_cast<Rank>(Pair_1), static_cast<Suit>(Comm_1), Hand))
								continue;

							Hand.emplace_back(static_cast<Suit>(Comm_1), static_cast<Rank>(Pair_1));

							for (unsigned int Comm_2 = 0; Comm_2 < ReferenceDeck.size(); Comm_2++)
							{
								if (IsCardInHand(ReferenceDeck[Comm_2], Hand) || ReferenceDeck[Comm_2].Get_Rank() == static_cast<Rank>(Pair_0) || ReferenceDeck[Comm_2].Get_Rank() == static_cast<Rank>(Pair_1))
									continue;

								Hand.push_back(ReferenceDeck[Comm_2]);

								if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreHandsIdentical(Hand, _ComparedTo); }) == Hands.end())
								{
									Hands.push_back(Hand);
									std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
									ComputeOdd(Hand, _OppoAmt, _Trials, File);
								}

								Hand.pop_back();
							}

							Hand.pop_back();
						}

						Hand.pop_back();
					}
				}

				Hand.pop_back();
			}

			Hand.clear();
		}
	}

	Hands.clear();

	//Hole Cards: 1 card of Pair A + 1 card of Pair B
	//Community: 1 card of Pair A + 1 card of Pair B + 1 random card of differing rank from A & B
	//Suit Restriction for Hole & Community: -
	//Mask: -
	for (unsigned int Pair_0 = 0; Pair_0 < 12; Pair_0++)
	{
		for (unsigned int Hole_0 = 0; Hole_0 < 4; Hole_0++)
		{
			Hand.emplace_back(static_cast<Suit>(Hole_0), static_cast<Rank>(Pair_0));

			for (unsigned int Pair_1 = Pair_0 + 1; Pair_1 < 13; Pair_1++)
			{
				if (Pair_1 == Pair_0)
					continue;
				
				for (unsigned int Hole_1 = 0; Hole_1 < 4; Hole_1++)
				{
					Hand.emplace_back(static_cast<Suit>(Hole_1), static_cast<Rank>(Pair_1));

					for (unsigned int Comm_0 = 0; Comm_0 < 4; Comm_0++)
					{
						if (IsCardInHand(static_cast<Rank>(Pair_0), static_cast<Suit>(Comm_0), Hand))
							continue;

						Hand.emplace_back(static_cast<Suit>(Comm_0), static_cast<Rank>(Pair_0));

						for (unsigned int Comm_1 = 0; Comm_1 < 4; Comm_1++)
						{
							if (IsCardInHand(static_cast<Rank>(Pair_1), static_cast<Suit>(Comm_1), Hand))
								continue;

							Hand.emplace_back(static_cast<Suit>(Comm_1), static_cast<Rank>(Pair_1));

							for(unsigned int Comm_2 = 0; Comm_2 < ReferenceDeck.size(); Comm_2++)
							{
								if (IsCardInHand(ReferenceDeck[Comm_2], Hand) || ReferenceDeck[Comm_2].Get_Rank() == static_cast<Rank>(Pair_0) || ReferenceDeck[Comm_2].Get_Rank() == static_cast<Rank>(Pair_1))
									continue;

								Hand.push_back(ReferenceDeck[Comm_2]);

								if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreHandsIdentical(Hand, _ComparedTo); }) == Hands.end())
								{
									Hands.push_back(Hand);
									std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
									ComputeOdd(Hand, _OppoAmt, _Trials, File);
								}

								Hand.pop_back();
							}

							Hand.pop_back();
						}

						Hand.pop_back();
					}

					Hand.pop_back();
				}
			}

			Hand.pop_back();
		}
	}

	Hands.clear();

	//Hole Cards: 1 card of Pair A + 1 random card
	//Community: 1 card of Pair A + 2 cards of Pair B
	//Suit restriction for Hole & Community: - 
	//Mask: - 
	for (unsigned int Pair_0 = 0; Pair_0 < 13; Pair_0++)
	{
		for (unsigned int Hole_0 = 0; Hole_0 < 4; Hole_0++)
		{
			Hand.emplace_back(static_cast<Suit>(Hole_0), static_cast<Rank>(Pair_0));

			for (unsigned int Hole_1 = 0; Hole_1 < ReferenceDeck.size(); Hole_1++)
			{
				if (IsCardInHand(ReferenceDeck[Hole_1], Hand) || ReferenceDeck[Hole_1].Get_Rank() == static_cast<Rank>(Pair_0))
					continue;

				Hand.push_back(ReferenceDeck[Hole_1]);

				for (unsigned int Comm_0 = 0; Comm_0 < 4; Comm_0++)
				{
					if (IsCardInHand(static_cast<Rank>(Pair_0), static_cast<Suit>(Comm_0), Hand))
						continue;

					Hand.emplace_back(static_cast<Suit>(Comm_0), static_cast<Rank>(Pair_0));

					for (unsigned int Pair_1 = 0; Pair_1 < 13; Pair_1++)
					{
						if (Pair_1 == Pair_0 || static_cast<Rank>(Pair_1) == ReferenceDeck[Hole_1].Get_Rank())
							continue;

						for (unsigned int Comm_1 = 0; Comm_1 < 3; Comm_1++)
						{
							Hand.emplace_back(static_cast<Suit>(Comm_1), static_cast<Rank>(Pair_1));

							for (unsigned int Comm_2 = Comm_1 + 1; Comm_2 < 4; Comm_2++)
							{
								Hand.emplace_back(static_cast<Suit>(Comm_2), static_cast<Rank>(Pair_1));

								if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreHandsIdentical(Hand, _ComparedTo); }) == Hands.end())
								{
									Hands.push_back(Hand);
									std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
									ComputeOdd(Hand, _OppoAmt, _Trials, File);
								}

								Hand.pop_back();
							}

							Hand.pop_back();
						}
					}

					Hand.pop_back();
				}

				Hand.pop_back();
			}

			Hand.clear();
		}
	}

	Hands.clear();*/

	//Pair
	//Hole Cards: Pocket Pair
	//Community; 3 random cards of differing rank
	//Suit Restriction: -
	//Mask: -
	/*for (unsigned int Pair_Rank = 4; Pair_Rank < 13; Pair_Rank++)
	{
		for (unsigned int Hole_0 = 0; Hole_0 < 3; Hole_0++)
		{
			Hand.emplace_back(static_cast<Suit>(Hole_0), static_cast<Rank>(Pair_Rank));

			for (unsigned int Hole_1 = Hole_0 + 1; Hole_1 < 4; Hole_1++)
			{
				Hand.emplace_back(static_cast<Suit>(Hole_1), static_cast<Rank>(Pair_Rank));

				for (unsigned int Comm_0 = 0; Comm_0 < ReferenceDeck.size() - 2; Comm_0++)
				{
					if (ReferenceDeck[Comm_0].Get_Rank() == static_cast<Rank>(Pair_Rank))
						continue;

					Hand.push_back(ReferenceDeck[Comm_0]);

					for (unsigned int Comm_1 = Comm_0 + 1; Comm_1 < ReferenceDeck.size() - 1; Comm_1++)
					{
						if (ReferenceDeck[Comm_1].Get_Rank() == ReferenceDeck[Comm_0].Get_Rank() || ReferenceDeck[Comm_1].Get_Rank() == static_cast<Rank>(Pair_Rank))
							continue;

						Hand.push_back(ReferenceDeck[Comm_1]);

						for (unsigned int Comm_2 = Comm_1 + 1; Comm_2 < ReferenceDeck.size(); Comm_2++)
						{
							if (ReferenceDeck[Comm_2].Get_Rank() == ReferenceDeck[Comm_0].Get_Rank() || ReferenceDeck[Comm_2].Get_Rank() == ReferenceDeck[Comm_1].Get_Rank() || ReferenceDeck[Comm_2].Get_Rank() == static_cast<Rank>(Pair_Rank))
								continue;

							Hand.push_back(ReferenceDeck[Comm_2]);

							if (std::find_if(Hands.begin(), Hands.end(), [&](std::vector<Card> _ComparedTo) { return AreHandsIdentical(Hand, _ComparedTo); }) == Hands.end())
							{
								Hands.push_back(Hand);
								std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
								ComputeOdd(Hand, _OppoAmt, _Trials, File);
							}

							Hand.pop_back();
						}

						Hand.pop_back();
					}

					Hand.pop_back();
				}

				Hand.pop_back();
			}

			Hand.clear();
			Hands.clear();
		}
	}*/

	//Hole Cards: 1 card of Pair + 1 random card of differing rank
	//Community: 1 card of Pair + 2 random cards of differing rank
	//Suit Restriction of Hole & Community: -
	//Mask: - 
	/*for (unsigned int Pair_Rank = 0; Pair_Rank < 13; Pair_Rank++)
	{
		for (unsigned int Hole_0 = 0; Hole_0 < 4; Hole_0++)
		{
			Hand.emplace_back(static_cast<Suit>(Hole_0), static_cast<Rank>(Pair_Rank));

			for (unsigned int Hole_1 = 0; Hole_1 < ReferenceDeck.size(); Hole_1++)
			{
				if (ReferenceDeck[Hole_1].Get_Rank() == static_cast<Rank>(Pair_Rank))
					continue;

				Hand.push_back(ReferenceDeck[Hole_1]);

				for (unsigned int Comm_0 = 0; Comm_0 < 4; Comm_0++)
				{
					if (Comm_0 == Hole_0)
						continue;

					Hand.emplace_back(static_cast<Suit>(Comm_0), static_cast<Rank>(Pair_Rank));

					for (unsigned int Comm_1 = 0; Comm_1 < ReferenceDeck.size() - 1; Comm_1++)
					{
						if (ReferenceDeck[Comm_1].Get_Rank() == ReferenceDeck[Hole_1].Get_Rank() || ReferenceDeck[Comm_1].Get_Rank() == static_cast<Rank>(Pair_Rank))
							continue;

						Hand.push_back(ReferenceDeck[Comm_1]);

						for (unsigned int Comm_2 = Comm_1; Comm_2 < ReferenceDeck.size(); Comm_2++)
						{
							if (ReferenceDeck[Comm_2].Get_Rank() == ReferenceDeck[Hole_1].Get_Rank() || ReferenceDeck[Comm_2].Get_Rank() == ReferenceDeck[Comm_1].Get_Rank() || ReferenceDeck[Comm_2].Get_Rank() == static_cast<Rank>(Pair_Rank))
								continue;

							Hand.push_back(ReferenceDeck[Comm_2]);

							std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
							ComputeOdd(Hand, _OppoAmt, _Trials, File);

							Hand.pop_back();
						}

						Hand.pop_back();
					}

					Hand.pop_back();
				}

				Hand.pop_back();
			}

			Hand.clear();
			//Hands.clear();
		}
	}*/

	//Hole Cards: 2 random cards of differing rank
	//Community: 1 random card of differing rank + 1 pair
	//Suit Restriction of Hole & Community: -
	//Mask: -
	/*for (unsigned int Hole_0 = 0; Hole_0 < ReferenceDeck.size() - 1; Hole_0++)
	{
		Hand.push_back(ReferenceDeck[Hole_0]);

		for (unsigned int Hole_1 = Hole_0 + 1; Hole_1 < ReferenceDeck.size(); Hole_1++)
		{
			if (ReferenceDeck[Hole_1].Get_Rank() == ReferenceDeck[Hole_0].Get_Rank())
				continue;

			Hand.push_back(ReferenceDeck[Hole_1]);

			for (unsigned int Comm_0 = 0; Comm_0 < ReferenceDeck.size(); Comm_0++)
			{
				if (ReferenceDeck[Comm_0].Get_Rank() == ReferenceDeck[Hole_1].Get_Rank() || ReferenceDeck[Comm_0].Get_Rank() == ReferenceDeck[Hole_0].Get_Rank())
					continue;

				Hand.push_back(ReferenceDeck[Comm_0]);

				for (unsigned int Pair_Rank = 0; Pair_Rank < 13; Pair_Rank++)
				{
					if (static_cast<Rank>(Pair_Rank) == ReferenceDeck[Comm_0].Get_Rank() || static_cast<Rank>(Pair_Rank) == ReferenceDeck[Hole_1].Get_Rank() || static_cast<Rank>(Pair_Rank) == ReferenceDeck[Hole_0].Get_Rank())
						continue;

					for (unsigned int Comm_1 = 0; Comm_1 < 3; Comm_1++)
					{
						Hand.emplace_back(static_cast<Suit>(Comm_1), static_cast<Rank>(Pair_Rank));

						for (unsigned int Comm_2 = Comm_1 + 1; Comm_2 < 4; Comm_2++)
						{
							Hand.emplace_back(static_cast<Suit>(Comm_2), static_cast<Rank>(Pair_Rank));

							std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
							ComputeOdd(Hand, _OppoAmt, _Trials, File);

							Hand.pop_back();
						}

						Hand.pop_back();
					}
				}

				Hand.pop_back();
			}

			Hand.pop_back();
		}

		Hand.clear();
	}*/

	//High Card
	//Hole Cards: 2 random cards of differing rank
	//Community: 3 random cards of differing rank
	//Suit restriction: No flush, at least 1 odd suit
	//Mask: -
	//*No common rank or straight are accepted
	for (unsigned int Hole_0 = 26; Hole_0 < ReferenceDeck.size() - 1; Hole_0++)
	{
		Hand.push_back(ReferenceDeck[Hole_0]);

		for (unsigned int Hole_1 = Hole_0 + 1; Hole_1 < ReferenceDeck.size(); Hole_1++)
		{
			if (ReferenceDeck[Hole_1].Get_Rank() == ReferenceDeck[Hole_0].Get_Rank())
				continue;

			Hand.push_back(ReferenceDeck[Hole_1]);

			for (unsigned int Comm_0 = 0; Comm_0 < ReferenceDeck.size() - 2; Comm_0++)
			{
				if (ReferenceDeck[Comm_0].Get_Rank() == ReferenceDeck[Hole_1].Get_Rank() || ReferenceDeck[Comm_0].Get_Rank() == ReferenceDeck[Hole_0].Get_Rank())
					continue;

				Hand.push_back(ReferenceDeck[Comm_0]);

				for (unsigned int Comm_1 = Comm_0 + 1; Comm_1 < ReferenceDeck.size() - 1; Comm_1++)
				{
					if (ReferenceDeck[Comm_1].Get_Rank() == ReferenceDeck[Comm_0].Get_Rank() || ReferenceDeck[Comm_1].Get_Rank() == ReferenceDeck[Hole_1].Get_Rank() || ReferenceDeck[Comm_1].Get_Rank() == ReferenceDeck[Hole_0].Get_Rank())
						continue;

					Hand.push_back(ReferenceDeck[Comm_1]);

					for (unsigned int Comm_2 = Comm_1 + 1; Comm_2 < ReferenceDeck.size(); Comm_2++)
					{
						if (ReferenceDeck[Comm_2].Get_Rank() == ReferenceDeck[Comm_1].Get_Rank() || ReferenceDeck[Comm_2].Get_Rank() == ReferenceDeck[Comm_0].Get_Rank() || ReferenceDeck[Comm_2].Get_Rank() == ReferenceDeck[Hole_1].Get_Rank() || ReferenceDeck[Comm_2].Get_Rank() == ReferenceDeck[Hole_0].Get_Rank())
							continue;

						Hand.push_back(ReferenceDeck[Comm_2]);

						if (!IsStraightValid(Hand) && !IsFlushValid(Hand))
						{
							std::cout << "Running " << Hand[0].To_String() << Hand[1].To_String() << " " << Hand[2].To_String() << Hand[3].To_String() << Hand[4].To_String() << "...\n";
							ComputeOdd(Hand, _OppoAmt, _Trials, File);
						}

						Hand.pop_back();
					}

					Hand.pop_back();
				}

				Hand.pop_back();
			}

			Hand.pop_back();
		}

		Hand.clear();
	}

	File.close();
}

void Precomputation::FillStraightGap(std::vector<Card>& _Current, Rank _Min, Rank _Max)
{
	for (unsigned int Index = (unsigned int)_Min; Index <= (unsigned int)_Max; Index++)
	{
		if (std::find_if(_Current.begin(), _Current.end(), [&](Card _Card) { return _Card.Get_Rank() == (Rank)Index; }) == _Current.end())
			_Current.emplace_back(Suit::Spade, static_cast<Rank>(Index));
	}
}

bool Precomputation::AreCardsIdentical(std::vector<Card> _First, std::vector<Card> _Second)
{
	for (auto const& Card_First : _First)
	{
		if (std::find_if(_Second.begin(), _Second.end(), [&](Card Card_Second) { return Card_Second.Get_Rank() == Card_First.Get_Rank() && Card_Second.Get_Suit() == Card_First.Get_Suit(); }) == _Second.end())
			return false;
	}

	return true;
}

bool Precomputation::AreCardsIdentical_Order(std::vector<Card> _First, std::vector<Card> _Second)
{
	if (_First.size() != _Second.size())
		return false;

	for (unsigned int Index = 0; Index < _First.size(); Index++)
	{
		if (_First[Index].Get_Rank() != _Second[Index].Get_Rank() || _First[Index].Get_Suit() != _Second[Index].Get_Suit())
			return false;
	}

	return true;
}

bool Precomputation::AreHandsIdentical(std::vector<Card> _First, std::vector<Card> _Second)
{
	if (_First.size() != _Second.size())
		return false;

	//std::cout << "Comparing Hands:\n";
	//std::cout << "1st: " << _First[0].To_String() << " " << _First[1].To_String() << " " << _First[2].To_String() << " " << _First[3].To_String() << " " << _First[4].To_String() << "\n";
	//std::cout << "2nd: " << _Second[0].To_String() << " " << _Second[1].To_String() << " " << _Second[2].To_String() << " " << _Second[3].To_String() << " " << _Second[4].To_String() << "\n";

	//std::cout << "Checking for Community:\n";
	for (auto CommItr = _First.rbegin(); CommItr != _First.rbegin() + 3; ++CommItr)
	{
		//std::cout << "Finding " << (*CommItr).To_String() << "...\n";

		if (std::find_if(_Second.rbegin(), _Second.rbegin() + 3, [&](Card _ComparedTo) { return (*CommItr).Get_Rank() == _ComparedTo.Get_Rank() && (*CommItr).Get_Suit() == _ComparedTo.Get_Suit(); }) == _Second.rbegin() + 3)
			//(std::find_if(_Second.begin() + 2, _Second.end(), [&](Card _ComparedTo) { return (*CommItr).Get_Rank() == _ComparedTo.Get_Rank() && (*CommItr).Get_Suit() == _ComparedTo.Get_Suit(); }) == _Second.end())
			return false;

		//std::cout << "Found !\n";
	}

	//std::cout << "Checking for Hole:\n";
	for (auto HoleItr = _First.rend() - 2; HoleItr != _First.rend(); ++HoleItr)
	{
		//std::cout << "Finding " << (*HoleItr).To_String() << "...\n";

		if (std::find_if(_Second.rend() - 2, _Second.rend(), [&](Card _ComparedTo) { return (*HoleItr).Get_Rank() == _ComparedTo.Get_Rank() && (*HoleItr).Get_Suit() == _ComparedTo.Get_Suit(); }) == _Second.rend())
			//std::find_if(_Second.begin(), _Second.begin() + 2, [&](Card _ComparedTo) { return (*HoleItr).Get_Rank() == _ComparedTo.Get_Rank() && (*HoleItr).Get_Suit() == _ComparedTo.Get_Suit(); }) == _Second.end())
			return false;

		//std::cout << "Found !\n";
	}

	std::cout << "Comparing Hands:\n";
	std::cout << "1st: " << _First[0].To_String() << " " << _First[1].To_String() << " " << _First[2].To_String() << " " << _First[3].To_String() << " " << _First[4].To_String() << "\n";
	std::cout << "2nd: " << _Second[0].To_String() << " " << _Second[1].To_String() << " " << _Second[2].To_String() << " " << _Second[3].To_String() << " " << _Second[4].To_String() << "\n";

	std::cout << "Had loop through community cards of \n";
	for (auto CommItr = _First.rbegin(); CommItr != _First.rbegin() + 3; ++CommItr)
	{
		std::cout << (*CommItr).To_String() << " ";
	}

	std::cout << "\nSearch Range: ";
	for (auto itr = _Second.rbegin(); itr != _Second.rbegin() + 3; itr++)
		std::cout << (*itr).To_String() << " ";

	std::cout << "\nHad loop through Hole cards of \n";
	for (auto HoleItr = _First.rend() - 2; HoleItr != _First.rend(); ++HoleItr)
	{
		std::cout << (*HoleItr).To_String() << " ";
	}

	std::cout << "\nSearch Range: ";
	for (auto itr = _Second.rend() - 2; itr != _Second.rend(); itr++)
		std::cout << (*itr).To_String() << " ";

	//std::cout << "\n";
	return true;

	/*std::vector<Card> Pocket_First(_First.begin(), _First.begin() + 2);
	std::vector<Card> Community_First(_First.begin() + 2, _First.end());
	std::vector<Card> Pocket_Second(_Second.begin(), _Second.begin() + 2);
	std::vector<Card> Community_Second(_Second.begin() + 2, _Second.end());

	//Are the pockets identical regardless of order?
	for (auto const& Card_First : Pocket_First)
	{
		if (std::find_if(Pocket_Second.begin(), Pocket_Second.end(), [&](Card _ComparedTo) { return Card_First.Get_Rank() == _ComparedTo.Get_Rank() && Card_First.Get_Suit() == _ComparedTo.Get_Suit(); }) == Pocket_Second.end())
				return false;
	}

	//Are the communities identical regards of order?
	for (auto const& Card_First : Community_First)
	{
		if (std::find_if(Community_Second.begin(), Community_Second.end(), [&](Card _ComparedTo) { return Card_First.Get_Rank() == _ComparedTo.Get_Rank() && Card_First.Get_Suit() == _ComparedTo.Get_Suit(); }) == Community_Second.end())
			return false;
	}

	return true;*/
}

bool Precomputation::IsCardInHand(Card _Card, std::vector<Card> _Hand)
{
	return std::find_if(_Hand.begin(), _Hand.end(), [&](Card _ComparedTo) { return _ComparedTo.Get_Rank() == _Card.Get_Rank() && _ComparedTo.Get_Suit() == _Card.Get_Suit(); }) != _Hand.end();
}

bool Precomputation::IsCardInHand(Rank _Rank, Suit _Suit, std::vector<Card> _Hand)
{
	return std::find_if(_Hand.begin(), _Hand.end(), [&](Card _ComparedTo) { return _ComparedTo.Get_Rank() == _Rank && _ComparedTo.Get_Suit() == _Suit; }) != _Hand.end();
}

bool Precomputation::IsCardInStraightRange(Card _Card, std::vector<Card> _Straight)
{
	for (auto const& ComparedTo : _Straight)
	{
		if (std::abs((int)ComparedTo.Get_Rank() - (int)_Card.Get_Rank()) > 4)
			return false;
	}

	return true;
}

bool Precomputation::IsStraightValid(std::vector<Card> _Straight)
{
	//Sort hand from lowest to highest rank
	std::sort(_Straight.begin(), _Straight.end(), [](Card _First, Card _Second) { return _First.Get_Rank() < _Second.Get_Rank(); });

	if (_Straight[4].Get_Rank() == Rank::Ace && _Straight[0].Get_Rank() == Rank::Two && _Straight[1].Get_Rank() == Rank::Three && _Straight[2].Get_Rank() == Rank::Four && _Straight[3].Get_Rank() == Rank::Five)
		return true;

	else if (((int)_Straight[4].Get_Rank() - (int)_Straight[0].Get_Rank()) != 4)
		return false;
	
	Rank CurrentRank = _Straight[0].Get_Rank();

	for (auto const& Card : _Straight)
	{
		if (CurrentRank != Card.Get_Rank())
			return false;
		else
			CurrentRank = CurrentRank == Rank::Ace ? Rank::Two : static_cast<Rank>((int)CurrentRank + 1);
	}

	return true;

	/*Rank PrevLowestRank = _Straight[0].Get_Rank();

	//Check if sequential ranks are avaliable
	for (auto const& Card : _Straight)
	{
		if (Card.Get_Rank() < PrevLowestRank)
			PrevLowestRank = Card.Get_Rank();
	}

	for (unsigned int Index = 0; Index < 4; Index++)
	{
		Rank NextRank = PrevLowestRank == Rank::Ace ? Rank::Two : static_cast<Rank>((int)PrevLowestRank + 1);
		
		if (std::find_if(_Straight.begin(), _Straight.end(), [&](Card _ComparedTo) { return _ComparedTo.Get_Rank() == NextRank; }) == _Straight.end())
			return false;
		else
			PrevLowestRank = NextRank;
	}

	//Check for rank wrapping (straight that goes through K, A, 2
	if (std::find_if(_Straight.begin(), _Straight.end(), [&](Card _ComparedTo) { return _ComparedTo.Get_Rank() == Rank::King; }) != _Straight.end() &&
		std::find_if(_Straight.begin(), _Straight.end(), [&](Card _ComparedTo) { return _ComparedTo.Get_Rank() == Rank::Two; }) != _Straight.end())
		return false;

	return true;*/
}

bool Precomputation::IsFlushValid(std::vector<Card> _Flush)
{
	for (auto const& Card : _Flush)
	{
		if (Card.Get_Suit() != _Flush[0].Get_Suit())
			return false;
	}

	return true;
}

void Precomputation::ComputeOdd(std::vector<Card> _Hand, unsigned int _MaxOppoAmt, unsigned int _Trials, std::ofstream& _File)
{
	std::array<Card, 2> Hole;
	std::vector<Card> Community;
	float Odds = 0.0f;

	if (_Hand.size() < 5)
		return;

	Hole[0] = _Hand[0];
	Hole[1] = _Hand[1];

	Community.insert(Community.end(), _Hand.begin() + 2, _Hand.end());

	_File << Hole[0].To_String() << Hole[1].To_String() << " ";
	for (auto const& Card : Community)
		_File << Card.To_String();
	_File << " ";

	for (unsigned int OppoIndex = 1; OppoIndex <= _MaxOppoAmt; OppoIndex++)
	{
		std::cout << "Against " << OppoIndex << "...\n";

		Odds = Evaluator->DetermineOdds_MonteCarlo_Multi_OMPEval(Hole, Community, OppoIndex, _Trials);
		_File << Odds << " ";
	}
	
	_File << std::endl;
}
