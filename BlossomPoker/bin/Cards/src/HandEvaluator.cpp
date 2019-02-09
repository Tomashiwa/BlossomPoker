#include "../inc/HandEvaluator.h"
#include "../../Tools/inc/xoroshiro128+.h"

HandEvaluator::HandEvaluator()
{
	s[0] = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	s[1] = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	Initialize();
}

void HandEvaluator::Initialize()
{
	memset(HR, 0, sizeof(HR));
	FILE * fin = fopen("HANDRANKS.DAT", "rb");
	size_t bytesread = fread(HR, sizeof(HR), 1, fin);
	fclose(fin);

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

	Eval = std::make_unique<omp::Evaluator>();

	std::ifstream OddsFile("PreflopOdds.txt");
	std::string EntryStr;

	while (std::getline(OddsFile, EntryStr))
	{
		std::string HoleStr = EntryStr.substr(0, EntryStr.find(' '));
		std::string OddsStr = EntryStr.substr(EntryStr.find(' ') + 1);

		OddsEntry Entry;
		Entry.Hole[0] = GetCardFromStr(HoleStr.substr(0, HoleStr.find('s') + 1));
		Entry.Hole[1] = GetCardFromStr(HoleStr.substr(HoleStr.find('s') + 1));

		std::istringstream Iss(OddsStr);
		for (std::string OddStr; Iss >> OddStr;)
			Entry.Odds.push_back(std::atof(OddStr.c_str()));

		/*if (HoleStr == "4s10h")
		{
			std::cout << "HoleStr: " << HoleStr << "\n";
			std::cout << "OddsStr: " << OddsStr << "\n";

			std::cout << "Substr 0: " << HoleStr.substr(0, HoleStr.find('s') + 1) << "\n";
			std::cout << "Substr 1: " << HoleStr.substr(HoleStr.find('s') + 1) << "\n";

			std::cout << "Hole 0: " << Entry.Hole[0].To_String() << "\n";
			std::cout << "Hole 1: " << Entry.Hole[1].To_String() << "\n";

			std::cout << "Odds: ";
			for (auto const Odd : Entry.Odds)
				std::cout << Odd << " ";
			std::cout << "\n\n";
		}*/

		PreflopOdds.push_back(Entry);
	}
}

void HandEvaluator::RandomFill(std::vector<Card>& _Set, std::vector<Card>& _Dead, std::size_t _Target)
{
	std::size_t RequiredAmt = _Target - _Set.size();

	auto Lamb_GenerateCard = [&]() {
		while (true)
		{
			Card NewCard = ReferenceDeck[next() % 51];
			bool IsValid = std::find_if(_Dead.begin(), _Dead.end(), [&](Card& _Card) { return _Card.Get_Rank() == NewCard.Get_Rank() && _Card.Get_Suit() == NewCard.Get_Suit(); }) == _Dead.end() &&
				std::find_if(_Set.begin(), _Set.end(), [&](Card& _Card) { return _Card.Get_Rank() == NewCard.Get_Rank() && _Card.Get_Suit() == NewCard.Get_Suit(); }) == _Set.end();
					
			if (IsValid)
				return NewCard;
		}
	};

	std::generate_n(std::back_inserter(_Set), RequiredAmt, Lamb_GenerateCard);
}

int HandEvaluator::GetCardInt_TwoPlusTwo(Card& _Card)
{
	return static_cast<int>(_Card);
}
 
int HandEvaluator::GetCardInt_OMPEval(Suit _Suit, Rank _Rank)//(const Card& _Card)
{
	return static_cast<int>(_Rank) * 4 - static_cast<int>(_Suit) + 3;
	//return static_cast<int>(_Card.Get_Rank()) * 4 - static_cast<int>(_Card.Get_Suit()) + 3;
}

std::array<int,5> HandEvaluator::Get5CardsInt_TwoPlusTwo(const std::array<Card, 5>& _Hand)
{
	std::array<int, 5> CardInts {};
	std::transform(_Hand.begin(), _Hand.end(), CardInts.begin(), [](Card _Card) { return static_cast<int>(_Card); });
	return CardInts;
}

std::array<int,7> HandEvaluator::Get7CardsInt_TwoPlusTwo(const std::array<Card,7>& _Hand)
{
	std::array<int, 7> CardInts{};
	std::transform(_Hand.begin(), _Hand.end(), CardInts.begin(), [](Card _Card) {return static_cast<int>(_Card); });
	return CardInts;
}

std::array<int, 7> HandEvaluator::Get7CardsInt_OMPEval(const std::array<Card, 7>& _Hand)
{
	std::array<int, 7> CardInts{};
	std::transform(_Hand.begin(), _Hand.end(), CardInts.begin(), [](Card card) { return static_cast<int>(card.Get_Rank()) * 4 - static_cast<int>(card.Get_Suit()) + 3;});

	return CardInts;
}

float HandEvaluator::DetermineOdds_Preflop(std::array<Card, 2> _Hole, unsigned int _OppoAmt)
{
	//std::cout << "Hole: " << _Hole[0].To_String() << ", " << _Hole[1].To_String() << "\n";
	
	//Determine type of Hole (ie. Pocket Pair, Suited Cards or Unsuited Cards)
	if (_Hole[0].Get_Rank() == _Hole[1].Get_Rank())
	{
		//std::cout << "They are pocket pairs with rank of " << ((int)_Hole[0].Get_Rank()) + 2 << "...\n";
		Rank PairRank = _Hole[0].Get_Rank();

		for (auto const& Entry : PreflopOdds)
		{
			if (Entry.Hole[0].Get_Rank() == PairRank && Entry.Hole[1].Get_Rank() == PairRank)
			{
				//std::cout << "Entry found with " << Entry.Hole[0].To_String() << "," << Entry.Hole[1].To_String() << "... Odds: " << Entry.Odds[0] << "," << Entry.Odds[1] << "," << Entry.Odds[2] << "," << Entry.Odds[3] << "," << Entry.Odds[4] << "," << Entry.Odds[5] << "," << Entry.Odds[6] << "\n";
				//std::cout << "Odds returned: " << Entry.Odds[_OppoAmt - 1] << "\n";
				return Entry.Odds[_OppoAmt - 1];
			}
		}

		return 0.0f;
	}
	else if (_Hole[0].Get_Suit() == _Hole[1].Get_Suit())
	{
		//std::cout << "They are suited cards with rank of " << ((int)_Hole[0].Get_Rank()) + 2 << "," << ((int) _Hole[1].Get_Rank()) - 2 << "...\n";

		for (auto const& Entry : PreflopOdds)
		{
			if (Entry.Hole[0].Get_Suit() == Entry.Hole[1].Get_Suit() && ((Entry.Hole[0].Get_Rank() == _Hole[0].Get_Rank() && Entry.Hole[1].Get_Rank() == _Hole[1].Get_Rank()) || (Entry.Hole[1].Get_Rank() == _Hole[0].Get_Rank() && Entry.Hole[0].Get_Rank() == _Hole[1].Get_Rank())))
			{
				//std::cout << "Entry found with " << Entry.Hole[0].To_String() << "," << Entry.Hole[1].To_String() << "... Odds: " << Entry.Odds[0] << "," << Entry.Odds[1] << "," << Entry.Odds[2] << "," << Entry.Odds[3] << "," << Entry.Odds[4] << "," << Entry.Odds[5] << "," << Entry.Odds[6] << "\n";				
				//std::cout << "Odds returned: " << Entry.Odds[_OppoAmt - 1] << "\n";
				return Entry.Odds[_OppoAmt - 1];
			}
		}

		return 0.0f;
	}

	//std::cout << "They are unsuited cards with rank of " << ((int)_Hole[0].Get_Rank()) + 2 << "," << ((int)_Hole[1].Get_Rank()) + 2 << "...\n";
	for (auto const& Entry : PreflopOdds)
	{
		if (Entry.Hole[0].Get_Suit() != Entry.Hole[1].Get_Suit() && ((Entry.Hole[0].Get_Rank() == _Hole[0].Get_Rank() && Entry.Hole[1].Get_Rank() == _Hole[1].Get_Rank()) || (Entry.Hole[1].Get_Rank() == _Hole[0].Get_Rank() && Entry.Hole[0].Get_Rank() == _Hole[1].Get_Rank())))
		{
			//std::cout << "Entry found with " << Entry.Hole[0].To_String() << "," << Entry.Hole[1].To_String() << "... Odds: " << Entry.Odds[0] << "," << Entry.Odds[1] << "," << Entry.Odds[2] << "," << Entry.Odds[3] << "," << Entry.Odds[4] << "," << Entry.Odds[5] << "," << Entry.Odds[6] << "\n";			
			//std::cout << "Odds returned: " << Entry.Odds[_OppoAmt - 1] << "\n";
			return Entry.Odds[_OppoAmt - 1];
		}
	}

	return 0.0f;
}

float HandEvaluator::DetermineOdds_MonteCarlo_Multi_TwoPlusTwo(std::array<Card, 2> _Hole, std::vector<Card> _Community, unsigned int _OppoAmt, unsigned int _TrialsAmt)
{
	std::array<Card, 7> PlayerHand{ _Hole[0], _Hole[1] };
	std::vector<std::array<Card, 7>> OpponentHands(_OppoAmt);
	std::vector<std::vector<Card>> OpponentHoles(_OppoAmt);

	std::vector<Card> Dead;
	std::vector<Card> Community(_Community);

	unsigned int PlayerScore = 0;
	std::vector<unsigned int> OpponentScores(_OppoAmt);

	unsigned int Win = 0, Draw = 0, GameCount = 0;
	unsigned int ExistingCount = Community.size();
	
	for (unsigned int Index = 0; Index < ExistingCount; Index++)
	{
		PlayerHand[2 + Index] = Community[Index];

		for (auto& Hand : OpponentHands)
			Hand[Index] = Community[Index];
	}

	Dead.insert(Dead.end(), _Hole.begin(), _Hole.end());
	Dead.insert(Dead.end(), Community.begin(), Community.end());
	
	//Pre-computation
	unsigned int CommunityScoreRef = DetermineValue_Cards(Community);
	unsigned int PlayerScoreRef = DetermineValue_Cards(std::vector<Card>(PlayerHand.begin(), PlayerHand.begin() + 2 + ExistingCount));

	for (unsigned int TrialIndex = 0; TrialIndex < _TrialsAmt; TrialIndex++)
	{
		RandomFill(Community, Dead, 5);
		Dead.insert(Dead.end(), Community.begin() + ExistingCount, Community.end());

		for (auto& Hole : OpponentHoles)
		{
			RandomFill(Hole, Dead, 2);
			Dead.insert(Dead.end(), Hole.begin(), Hole.end());
		}

		for (unsigned int Index = ExistingCount, Max = Community.size(); Index < Max; Index++)
			PlayerHand[Index + 2] = Community[Index];

		for (unsigned int Index = 0; Index < _OppoAmt; Index++)
		{
			OpponentHands[Index][ExistingCount] = OpponentHoles[Index][0];
			OpponentHands[Index][ExistingCount + 1] = OpponentHoles[Index][1];

			for (unsigned int CommIndex = ExistingCount, Max = Community.size(); CommIndex < Max; CommIndex++)
				OpponentHands[Index][CommIndex + 2] = Community[CommIndex];
		}

		PlayerScore = DetermineValue_7Cards_TwoPlusTwo(PlayerHand, PlayerScoreRef, 2 + ExistingCount);

		for (unsigned int Index = 0; Index < _OppoAmt; Index++)
			OpponentScores[Index] = DetermineValue_7Cards_TwoPlusTwo(OpponentHands[Index], CommunityScoreRef, ExistingCount);

		bool HasPlayerWin = true;
		for (auto& Score : OpponentScores)
		{
			if (PlayerScore < Score)
			{
				HasPlayerWin = false;
				break;
			}
		}

		if (HasPlayerWin)
			Win++;

		GameCount++;

		Dead.erase(Dead.begin() + 2 + ExistingCount, Dead.end());
		Community.erase(Community.begin() + ExistingCount, Community.end());

		for (auto& Hole : OpponentHoles)
			Hole.clear();
	}

	//std::cout << "\nWin Rate: " << Win << "/" << GameCount << " = " << ((float)Win / (float)GameCount) * 100.0f;
	return ((float)Win / (float)GameCount) * 100.0f;//(((float)Win) + ((float)Draw) / 2.0f) / ((float)GameCount) * 100.0f;
}

float HandEvaluator::DetermineOdds_MonteCarlo_Multi_OMPEval(std::array<Card, 2> _Hole, std::vector<Card> _Community, unsigned int _OppoAmt, unsigned int _TrialsAmt)
{
	std::array<Card, 7> PlayerHand{ _Hole[0], _Hole[1] };
	std::vector<std::array<Card, 7>> OpponentHands(_OppoAmt);
	std::vector<std::vector<Card>> OpponentHoles(_OppoAmt);

	std::vector<Card> Dead;
	std::vector<Card> Community(_Community);

	unsigned int PlayerScore = 0;
	std::vector<unsigned int> OpponentScores(_OppoAmt);

	unsigned int Win = 0, Draw = 0, GameCount = 0;
	unsigned int ExistingCount = Community.size();

	for (unsigned int Index = 0; Index < ExistingCount; Index++)
	{
		PlayerHand[2 + Index] = Community[Index];

		for (auto& Hand : OpponentHands)
			Hand[Index] = Community[Index];
	}

	Dead.insert(Dead.end(), _Hole.begin(), _Hole.end());
	Dead.insert(Dead.end(), Community.begin(), Community.end());

	for (unsigned int TrialIndex = 0; TrialIndex < _TrialsAmt; TrialIndex++)
	{
		RandomFill(Community, Dead, 5);
		Dead.insert(Dead.end(), Community.begin() + ExistingCount, Community.end());

		for (auto& Hole : OpponentHoles)
		{
			RandomFill(Hole, Dead, 2);
			Dead.insert(Dead.end(), Hole.begin(), Hole.end());
		}

		for (unsigned int Index = ExistingCount, Max = Community.size(); Index < Max; Index++)
			PlayerHand[Index + 2] = Community[Index];

		for (unsigned int Index = 0; Index < _OppoAmt; Index++)
		{
			OpponentHands[Index][ExistingCount] = OpponentHoles[Index][0];
			OpponentHands[Index][ExistingCount + 1] = OpponentHoles[Index][1];

			for (unsigned int CommIndex = ExistingCount, Max = Community.size(); CommIndex < Max; CommIndex++)
				OpponentHands[Index][CommIndex + 2] = Community[CommIndex];
		}

		PlayerScore = DetermineValue_7Cards_OMPEval(PlayerHand);

		for (unsigned int Index = 0; Index < _OppoAmt; Index++)
			OpponentScores[Index] = DetermineValue_7Cards_OMPEval(OpponentHands[Index]);

		bool HasPlayerWin = true;
		for (auto& Score : OpponentScores)
		{
			if (PlayerScore < Score)
			{
				HasPlayerWin = false;
				break;
			}
		}

		if (HasPlayerWin)
			Win++;

		GameCount++;

		Dead.erase(Dead.begin() + 2 + ExistingCount, Dead.end());
		Community.erase(Community.begin() + ExistingCount, Community.end());

		for (auto& Hole : OpponentHoles)
			Hole.clear();
	}

	//std::cout << "\nWin Rate: " << Win << "/" << GameCount << " = " << ((float)Win / (float)GameCount) * 100.0f;
	return ((float)Win / (float)GameCount) * 100.0f;//(((float)Win) + ((float)Draw) / 2.0f) / ((float)GameCount) * 100.0f;
}

int HandEvaluator::DetermineValue_Cards(const std::vector<Card>& _Cards)
{
	if (_Cards.size() == 0)
		return 0;

	std::vector<int> CardInts;
	for (auto Card : _Cards)
		CardInts.push_back(static_cast<int>(Card));
	
	int Value = HR[53 + CardInts[0]];
	for (unsigned int Index = 1; Index < CardInts.size(); Index++)
		Value = HR[Value + CardInts[Index]];

	return Value;
}

int HandEvaluator::DetermineValue_5Cards_TwoPlusTwo(const std::array<Card, 5>& _Hand)
{
	std::array<int, 5> CardInts = Get5CardsInt_TwoPlusTwo(_Hand);
	
	int *Value1 = HR + HR[53 + CardInts[0]];
	int *Value2 = HR + Value1[CardInts[1]];
	int *Value3 = HR + Value2[CardInts[2]];
	int *Value4 = HR + Value3[CardInts[3]];
	int *Value5 = HR + Value4[CardInts[4]];

	return Value5[0]; 
}

int HandEvaluator::DetermineValue_5Cards_OMPEval(const std::array<Card, 5>& _Hand)
{
	omp::Hand SampleHand = omp::Hand::empty();
	SampleHand += omp::Hand(GetCardInt_OMPEval(_Hand[0].Get_Suit(), _Hand[0].Get_Rank())) + omp::Hand(GetCardInt_OMPEval(_Hand[1].Get_Suit(), _Hand[1].Get_Rank())) + omp::Hand(GetCardInt_OMPEval(_Hand[2].Get_Suit(), _Hand[2].Get_Rank())) + omp::Hand(GetCardInt_OMPEval(_Hand[3].Get_Suit(), _Hand[3].Get_Rank())) + omp::Hand(GetCardInt_OMPEval(_Hand[4].Get_Suit(), _Hand[4].Get_Rank()));
	return Eval->evaluate(SampleHand);
}

int HandEvaluator::DetermineValue_7Cards_TwoPlusTwo(const std::array<Card, 7>& _Hand, int _PrecomputeScore, unsigned int _ContinueFrom)
{	
	std::array<int, 7> CardInts = Get7CardsInt_TwoPlusTwo(_Hand);

	if (_PrecomputeScore != -1 && _ContinueFrom >= 1)
	{
		switch (_ContinueFrom)
		{
		case 1:
			return HR[HR[HR[HR[HR[HR[HR[_PrecomputeScore + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]] + CardInts[6]]];
			break;
		case 2:
			return HR[HR[HR[HR[HR[_PrecomputeScore + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]] + CardInts[6]];
			break;
		case 3:
			return HR[HR[HR[HR[_PrecomputeScore + CardInts[3]] + CardInts[4]] + CardInts[5]] + CardInts[6]];
			break;
		case 4:
			return HR[HR[HR[_PrecomputeScore + CardInts[4]] + CardInts[5]] + CardInts[6]];
			break;
		case 5:
			return HR[HR[_PrecomputeScore + CardInts[5]] + CardInts[6]];
			break;
		case 6:
			return HR[_PrecomputeScore + CardInts[6]];
			break;
		default:
			return HR[HR[HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]] + CardInts[6]];
			break;
		}
	}

	return HR[HR[HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]] + CardInts[6]];
}

int HandEvaluator::DetermineValue_7Cards_OMPEval(const std::array<Card, 7>& _Hand)
{
	omp::Hand SampleHand = omp::Hand::empty();
	SampleHand += omp::Hand(GetCardInt_OMPEval(_Hand[0].Get_Suit(), _Hand[0].Get_Rank())) + omp::Hand(GetCardInt_OMPEval(_Hand[1].Get_Suit(), _Hand[1].Get_Rank())) + omp::Hand(GetCardInt_OMPEval(_Hand[2].Get_Suit(), _Hand[2].Get_Rank())) + omp::Hand(GetCardInt_OMPEval(_Hand[3].Get_Suit(), _Hand[3].Get_Rank())) + omp::Hand(GetCardInt_OMPEval(_Hand[4].Get_Suit(), _Hand[4].Get_Rank())) + omp::Hand(GetCardInt_OMPEval(_Hand[5].Get_Suit(), _Hand[5].Get_Rank())) + omp::Hand(GetCardInt_OMPEval(_Hand[6].Get_Suit(), _Hand[6].Get_Rank()));
	return Eval->evaluate(SampleHand);
}

Hand HandEvaluator::DetermineType(int _Value)
{
	int Type = _Value >> 12;

	switch(Type)
	{
		case 9:
			return _Value == 36874 ? Hand::RoyalFlush : Hand::StraightFlush;
		case 8:
			return Hand::FourKind;
		case 7:
			return Hand::FullHouse;
		case 6:
			return Hand::Flush;
		case 5:
			return Hand::Straight;
		case 4:
			return Hand::ThreeKind;
		case 3:
			return Hand::TwoPair;
		case 2:
			return Hand::Pair;
		case 1:
			return Hand::High;
	}

	return Hand::High;
}

ComparisonResult HandEvaluator::IsBetter5Cards(std::array<Card, 5> _First, std::array<Card, 5> _Second)
{
	int FirstValue = DetermineValue_5Cards_OMPEval(_First);
	int SecondValue = DetermineValue_5Cards_OMPEval(_Second); 

	if (FirstValue == SecondValue)
		return ComparisonResult::Draw;

	return FirstValue > SecondValue ? ComparisonResult::Win : ComparisonResult::Lose;
}

ComparisonResult HandEvaluator::IsBetter7Cards(std::array<Card, 7> _First, std::array<Card, 7> _Second)
{
	int FirstValue = DetermineValue_7Cards_OMPEval(_First);
	int SecondValue = DetermineValue_7Cards_OMPEval(_Second);

	if (FirstValue == SecondValue)
		return ComparisonResult::Draw;

	return FirstValue > SecondValue ? ComparisonResult::Win : ComparisonResult::Lose;
}

ComparisonResult HandEvaluator::IsWorse5Cards(std::array<Card, 5> _First, std::array<Card, 5> _Second)
{
	return IsBetter5Cards(_Second, _First);
}

std::array<Card, 5> HandEvaluator::GetBestCommunalHand(std::array<Card, 2> _Hole, std::vector<Card> _Community)
{
	std::array<Card, 5> BestHand;

	if (_Community.size() == 3)
	{
		std::copy(_Hole.begin(), _Hole.end(), BestHand.begin());
		std::copy(_Community.begin(), _Community.end(), BestHand.begin() + 2);

		return BestHand;
	}
	else if (_Community.size() == 4)
	{
		std::vector<std::array<Card, 5>> PossibleHands
		{
			{ _Community[0], _Community[1], _Community[2], _Community[3], _Hole[0] },
			{ _Community[0], _Community[1], _Community[2], _Community[3], _Hole[1] },
			{ _Community[0], _Community[1], _Community[2], _Hole[0], _Hole[1] },
			{_Community[0], _Community[1], _Community[2], _Hole[0], _Hole[1]},
			{_Community[0], _Community[1], _Community[2], _Hole[0], _Hole[1]},
			{_Community[0], _Community[1], _Community[2], _Hole[0], _Hole[1]}
		};

		BestHand = PossibleHands[0];

		for (auto const& Hand : PossibleHands)
		{
			if (IsBetter5Cards(Hand, BestHand) == ComparisonResult::Win)
				BestHand = Hand;
		}

		return BestHand;
	}

	//5 community cards
	std::copy(_Community.begin(), _Community.end(), BestHand.begin());
	std::vector<std::array<Card, 5>> PossibleHands
	{
		//Combination with first or second hole card
		{ _Hole[0], _Community[0], _Community[1], _Community[2], _Community[3] },
		{ _Hole[0], _Community[0], _Community[1], _Community[2], _Community[4] },
		{ _Hole[0], _Community[0], _Community[1], _Community[3], _Community[4] },
		{ _Hole[0], _Community[0], _Community[2], _Community[3], _Community[4] },
		{ _Hole[0], _Community[1], _Community[2], _Community[3], _Community[4] },
		
		{ _Hole[1], _Community[0], _Community[1], _Community[2], _Community[3] },
		{ _Hole[1], _Community[0], _Community[1], _Community[2], _Community[4] },
		{ _Hole[1], _Community[0], _Community[1], _Community[3], _Community[4] },
		{ _Hole[1], _Community[0], _Community[2], _Community[3], _Community[4] },
		{ _Hole[1], _Community[1], _Community[2], _Community[3], _Community[4] },

		//Combination with both hole cards
		{ _Hole[0], _Hole[1], _Community[0], _Community[1], _Community[2] },
		{ _Hole[0], _Hole[1], _Community[0], _Community[1], _Community[3] },
		{ _Hole[0], _Hole[1], _Community[0], _Community[1], _Community[4] },
		{ _Hole[0], _Hole[1], _Community[0], _Community[2], _Community[3] },
		{ _Hole[0], _Hole[1], _Community[0], _Community[2], _Community[4] },
		{ _Hole[0], _Hole[1], _Community[0], _Community[3], _Community[4] },
		{ _Hole[0], _Hole[1], _Community[1], _Community[2], _Community[3] },
		{ _Hole[0], _Hole[1], _Community[1], _Community[2], _Community[4] },
		{ _Hole[0], _Hole[1], _Community[1], _Community[3], _Community[4] },
		{ _Hole[0], _Hole[1], _Community[2], _Community[3], _Community[4] }
	};

	for (auto const& Hand : PossibleHands)
	{
		if (IsBetter5Cards(Hand, BestHand) == ComparisonResult::Win)
			BestHand = Hand;
	}

	return BestHand;
}

std::string HandEvaluator::GetTypeStr(Hand _Hand)
{
	switch (_Hand)
	{
		case Hand::RoyalFlush:
			return "Royal Flush";
		case Hand::StraightFlush:
			return "Straight Flush";
		case Hand::FourKind:
			return "Four of a Kind";
		case Hand::FullHouse:
			return "Full House";
		case Hand::Flush:
			return "Flush";
		case Hand::Straight:
			return "Straight";
		case Hand::ThreeKind:
			return "Three of a kind";
		case Hand::TwoPair:
			return "Two Pair";
		case Hand::Pair:
			return "Pair";
		case Hand::High:
			return "High";

		default:
			return "NONE";
	}
}

std::string HandEvaluator::GetTypeStr(std::array<Card, 5> _Hand)
{
	return GetTypeStr(DetermineType(DetermineValue_5Cards_TwoPlusTwo(_Hand)));
}

std::string HandEvaluator::GetStr(std::array<Card, 2> _Hole)
{
	return _Hole[0].To_String() + " " + _Hole[1].To_String();
}

std::string HandEvaluator::GetStr(std::array<Card, 5> _Hand)
{
	return _Hand[0].To_String() + " " + _Hand[1].To_String() + " " + _Hand[2].To_String() + " " + _Hand[3].To_String() + " " + _Hand[4].To_String();
}

std::string HandEvaluator::GetStr(std::array<Card, 7> _Hand)
{
	return _Hand[0].To_String() + " " + _Hand[1].To_String() + " " + _Hand[2].To_String() + " " + _Hand[3].To_String() + " " + _Hand[4].To_String() + " " + _Hand[5].To_String() + " " + _Hand[6].To_String();
}

std::string HandEvaluator::GetStr(std::vector<Card> _Hand)
{
	std::string str = "";

	for (auto const& Card : _Hand)
		str += Card.To_String() + " ";

	return str;
}

Card HandEvaluator::GetCardFromStr(std::string _Text)
{
	if (_Text.size() == 3)
	{
		unsigned int SuitVal = 0;
		
		switch (_Text[2])
		{
		case 'c':
			SuitVal = 0;
			break;
		case 'd':
			SuitVal = 1;
			break;
		case 'h':
			SuitVal = 2;
			break;
		case 's':
			SuitVal = 3;
			break;
		}

		std::string RankStr = _Text.substr(0, 2);
		unsigned int RankVal = 0;

		if (RankStr == "A")
			RankVal = 12;
		else if (RankStr == "K")
			RankVal = 11;
		else if (RankStr == "Q")
			RankVal = 10;
		else if (RankStr == "J")
			RankVal = 9;
		else 
			RankVal = std::stoul(RankStr) - 2;

		return Card((Suit)SuitVal, (Rank)RankVal);
	}

	unsigned int RankVal = 0;

	switch (_Text[0])
	{
	case 'A':
		RankVal = 12;
		break;
	case 'K':
		RankVal = 11;
		break;
	case 'Q':
		RankVal = 10;
		break;
	case 'J':
		RankVal = 9;
		break;
	default:
		RankVal = static_cast<unsigned int>(_Text[0]) - 48 - 2;
		break;
	}

	unsigned int SuitVal = 0;

	switch (_Text[1])
	{
	case 'c':
		SuitVal = 0;
		break;
	case 'd':
		SuitVal = 1;
		break;
	case 'h':
		SuitVal = 2;
		break;
	case 's':
		SuitVal = 3;
		break;
	}

	return Card((Suit)SuitVal, (Rank)RankVal);
}