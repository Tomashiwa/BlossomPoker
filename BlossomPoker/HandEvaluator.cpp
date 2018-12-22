#include "HandEvaluator.h"
#include "xoroshiro128+.h"

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

	std::cout << "\n";
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

	/*//Add the cards that are currently in Set as dead cards
	for (auto const& Card : _Set)
		_Dead.push_back(Card);

	bool IsDead;
	unsigned int RequiredAmt = _Target - _Set.size();
	//std::uniform_int_distribution<unsigned int> CardsDistribution(0, 51);

	for (unsigned int Index = 0; Index < RequiredAmt; Index++)
	{
		while (true)
		{
			//_Set.push_back(ReferenceDeck[CardsDistribution(MTGenerator)]);
			_Set.push_back(ReferenceDeck[next() % 52]);

			IsDead = false;

			for (auto const& Dead : _Dead)
			{
				if (Dead == _Set[_Set.size() - 1])
				{
					IsDead = true;
					break;
				}
			}

			if (IsDead)
				_Set.pop_back();
			else
				break;
		}
	}*/
}
 
int HandEvaluator::GetCardInt(Card& _Card)
{
	return static_cast<int>(_Card);
}

std::array<int,5> HandEvaluator::Get5CardsInt(const std::array<Card, 5>& _Hand)
{
	std::array<int, 5> CardInts {};
	std::transform(_Hand.begin(), _Hand.end(), CardInts.begin(), [](Card _Card) { return static_cast<int>(_Card); });
	return CardInts;
}

std::array<int,7> HandEvaluator::Get7CardsInt(const std::array<Card,7>& _Hand)
{
	std::array<int, 7> CardInts{};
	std::transform(_Hand.begin(), _Hand.end(), CardInts.begin(), [](Card _Card) {return static_cast<int>(_Card); });
	return CardInts;
}

float HandEvaluator::DetermineOdds_MonteCarlo_Multi(std::array<Card, 2> _Hole, std::vector<Card> _Community, unsigned int _OppoAmt, unsigned int _TrialsAmt)
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

		PlayerScore = DetermineValue_7Cards(PlayerHand);

		for (unsigned int Index = 0; Index < _OppoAmt; Index++)
			OpponentScores[Index] = DetermineValue_7Cards(OpponentHands[Index]);

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

int HandEvaluator::DetermineValue_5Cards(const std::array<Card, 5>& _Hand)
{
	std::array<int, 5> CardInts = Get5CardsInt(_Hand);
	
	int *Value1 = HR + HR[53 + CardInts[0]];
	int *Value2 = HR + Value1[CardInts[1]];
	int *Value3 = HR + Value2[CardInts[2]];
	int *Value4 = HR + Value3[CardInts[3]];
	int *Value5 = HR + Value4[CardInts[4]];

	return Value5[0]; 
}

int HandEvaluator::DetermineValue_7Cards(const std::array<Card, 7>& _Hand)
{
	/*int* Cards = Get7CardsInt(_Hand).data();
	int Score = HR[53 + *Cards++];
	Score = HR[Score + *Cards++];
	Score = HR[Score + *Cards++];
	Score = HR[Score + *Cards++];
	Score = HR[Score + *Cards++];
	Score = HR[Score + *Cards++];
	Score = HR[Score + *Cards++];

	std::cout << "Result Value: " << Score << "\n\n";

	return HR[Score + *Cards++];*/
	
	std::array<int, 7> CardInts = Get7CardsInt(_Hand);

	//std::cout << "HR[53 + CardInts[0]]: " << HR[53 + CardInts[0]] << "\n";
	//std::cout << "HR[53 + CardInts[0]] + CardInts[1]]: " << HR[HR[53 + CardInts[0]] + CardInts[1]] << "\n";
	//std::cout << "HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]]: " << HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] << "\n";
	//std::cout << "HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]]: " << HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] << "\n";
	//std::cout << "HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]]: " << HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] << "\n";
	//std::cout << "HR[HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]]: " << HR[HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]] << "\n";
	//std::cout << "HR[HR[HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]] + CardInts[6]]: " << HR[HR[HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]] + CardInts[6]] << "\n";

	return HR[HR[HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]] + CardInts[6]];
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
	int FirstValue = DetermineValue_5Cards(_First);
	int SecondValue = DetermineValue_5Cards(_Second); 

	if (FirstValue == SecondValue)
		return ComparisonResult::Draw;

	return FirstValue > SecondValue ? ComparisonResult::Win : ComparisonResult::Lose;
}

ComparisonResult HandEvaluator::IsBetter7Cards(std::array<Card, 7> _First, std::array<Card, 7> _Second)
{
	int FirstValue = DetermineValue_7Cards(_First);
	int SecondValue = DetermineValue_7Cards(_Second);

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
		std::vector<std::array<Card, 5>> PossibleHands;

		PossibleHands.push_back(*new std::array<Card, 5>{_Community[0], _Community[1], _Community[2], _Community[3], _Hole[0]});
		PossibleHands.push_back(*new std::array<Card, 5>{_Community[0], _Community[1], _Community[2], _Community[3], _Hole[1]});
		PossibleHands.push_back(*new std::array<Card, 5>{_Community[0], _Community[1], _Community[2], _Hole[0], _Hole[1]});
		PossibleHands.push_back(*new std::array<Card, 5>{_Community[0], _Community[1], _Community[3], _Hole[0], _Hole[1]});
		PossibleHands.push_back(*new std::array<Card, 5>{_Community[0], _Community[2], _Community[3], _Hole[0], _Hole[1]});
		PossibleHands.push_back(*new std::array<Card, 5>{_Community[1], _Community[2], _Community[3], _Hole[0], _Hole[1]});

		BestHand = PossibleHands[0];

		for (auto const& Hand : PossibleHands)
		{
			if (IsBetter5Cards(Hand, BestHand) == ComparisonResult::Win)
				BestHand = Hand;
		}

		return BestHand;
	}

	//5 community cards
	std::vector<std::array<Card, 5>> PossibleHands;
	std::copy(_Community.begin(), _Community.end(), BestHand.begin());

	//Combination with first or second hole card
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Community[0], _Community[1], _Community[2], _Community[3] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Community[0], _Community[1], _Community[2], _Community[4] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Community[0], _Community[1], _Community[3], _Community[4] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Community[0], _Community[2], _Community[3], _Community[4] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Community[1], _Community[2], _Community[3], _Community[4] });

	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[1], _Community[0], _Community[1], _Community[2], _Community[3] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[1], _Community[0], _Community[1], _Community[2], _Community[4] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[1], _Community[0], _Community[1], _Community[3], _Community[4] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[1], _Community[0], _Community[2], _Community[3], _Community[4] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[1], _Community[1], _Community[2], _Community[3], _Community[4] });

	//Combination with both hole cards	 
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Hole[1], _Community[0], _Community[1], _Community[2] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Hole[1], _Community[0], _Community[1], _Community[3] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Hole[1], _Community[0], _Community[1], _Community[4] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Hole[1], _Community[0], _Community[2], _Community[3] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Hole[1], _Community[0], _Community[2], _Community[4] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Hole[1], _Community[0], _Community[3], _Community[4] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Hole[1], _Community[1], _Community[2], _Community[3] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Hole[1], _Community[1], _Community[2], _Community[4] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Hole[1], _Community[1], _Community[3], _Community[4] });
	PossibleHands.push_back(*new std::array<Card, 5>{ _Hole[0], _Hole[1], _Community[2], _Community[3], _Community[4] });

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
	return GetTypeStr(DetermineType(DetermineValue_5Cards(_Hand)));
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