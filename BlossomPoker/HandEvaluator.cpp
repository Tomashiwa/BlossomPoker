#include "HandEvaluator.h"
#include "Card.h"

HandEvaluator::HandEvaluator()
{
	Initialize();
}

HandEvaluator::~HandEvaluator()
{
}

void HandEvaluator::ConvertPreflopOddsTxt()
{
	std::vector<std::array<std::string, 2>> Data;
	std::array<std::string, 2> Entry;

	std::ifstream inTxtFile("PrefopOdds.txt");
	std::string str_First, str_Second, str_Odds, str_PairInt;
	
	while (inTxtFile >> str_First >> str_Second >> str_Odds)
	{
		std::cout << "Cards: " << str_First << "," << str_Second << " | Odds: " << str_Odds << std::endl;

		Entry.empty();
		Entry[0] = std::to_string(GetCardInt(str_First) + GetCardInt(str_Second));
		Entry[1] = str_Odds;
	
		std::cout << "PairInt: " << Entry[0] << " | Odds: " << Entry[1] << std::endl;

		Data.push_back(Entry);
	}

	std::ofstream outfile("PreflopResults.txt");

	for (unsigned int Index = 0; Index < Data.size(); Index++)
		outfile << Data[Index][0] << " " << Data[Index][1] << std::endl;

	outfile.close();
}

void HandEvaluator::Test()
{
	int Hand1[] = { 4, 10, 17, 25, 35 };
	int Hand2[] = { 38, 49, 7, 19, 23 };

	int *H1Value1 = HR + HR[53 + Hand1[0]];
	int *H1Value2 = HR + H1Value1[Hand1[1]];
	int *H1Value3 = HR + H1Value2[Hand1[2]];
	int *H1Value4 = HR + H1Value3[Hand1[3]];
	int *H1Value5 = HR + H1Value4[Hand1[4]];

	std::cout << "H1's HV: " << H1Value5[0] << std::endl;
		  
	int *H2Value1 = HR + HR[53 + Hand2[0]];
	int *H2Value2 = HR + H2Value1[Hand2[1]];
	int *H2Value3 = HR + H2Value2[Hand2[2]];
	int *H2Value4 = HR + H2Value3[Hand2[3]];
	int *H2Value5 = HR + H2Value4[Hand2[4]];

	std::cout << "H2's HV: " << H2Value5[0] << std::endl;
}

void HandEvaluator::Initialize()
{
	memset(HR, 0, sizeof(HR));
	FILE * fin = fopen("HANDRANKS.DAT", "rb");
	size_t bytesread = fread(HR, sizeof(HR), 1, fin);
	//printf("read %zu bytes\n", bytesread * sizeof(*HR));
	fclose(fin);

	//std::cout << "Eval Initialized..." << std::endl;
}

void HandEvaluator::RandomFill(std::vector<Card*>& _Set, std::vector<Card*> _Dead, int _Target)
{
	//Add the cards that are currently in Set as dead cards
	if (!_Set.empty())
	{
		for (auto Index = 0u; Index < _Set.size(); Index++)
		{
			if (_Set[Index] == nullptr)
				break;

			_Dead.push_back(_Set[Index]);
		}
	}

	//Get a generic deck composision and remove the dead cards
	std::vector<Card*> PossibleCards = GetDeckComposition();

	for (auto Index = 0u; Index < _Dead.size(); Index++)
	{
		PossibleCards.erase(std::remove_if(PossibleCards.begin(), 
							PossibleCards.end(), [&](Card* _Card) {return _Card->GetSuit() == _Dead[Index]->GetSuit() && _Card->GetRank() == _Dead[Index]->GetRank();}), 
							PossibleCards.end());
	}

	int RequiredAmt = _Target - _Set.size();

	//Use Mersenne Twister to obtain a random card to insert into Set
	auto Seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	
	std::mt19937 mt(Seed);
	std::uniform_int_distribution<int> CardsDistribution(0, PossibleCards.size() - 1); 

	for (auto Index = 0; Index < RequiredAmt; Index++)
	{
		CardsDistribution.param(std::uniform_int_distribution<int>::param_type(0, PossibleCards.size() - 1));
		auto RandomIndex = CardsDistribution(mt);
		
		_Set.push_back(PossibleCards[RandomIndex]);
		PossibleCards.erase(PossibleCards.begin() + RandomIndex);
	}
}

int HandEvaluator::GetCardInt(std::string _CardTxt)
{
	int RankInt = -1, SuitInt = -1;

	char SuitType = _CardTxt.back();
	std::string RankType = _CardTxt.substr(0, _CardTxt.size() - 1);

	switch (SuitType)
	{
		case 's':
			SuitInt = static_cast<int>(Suit::Spade);
			break;
		case 'h':
			SuitInt = static_cast<int>(Suit::Heart);
			break;
		case 'd':
			SuitInt = static_cast<int>(Suit::Diamond);
			break;
		case 'c':
			SuitInt = static_cast<int>(Suit::Club);
			break;
		default:
			SuitInt = -1;
			break;
	}

	if (RankType == "J")
		RankInt = static_cast<int>(Rank::Jack);
	else if (RankType == "Q")
		RankInt = static_cast<int>(Rank::Queen);
	else if (RankType == "K")
		RankInt = static_cast<int>(Rank::King);
	else if (RankType == "A")
		RankInt = static_cast<int>(Rank::Ace);
	else
		RankInt = std::stoi(RankType) - 2;

	if (SuitInt == -1 || RankInt == -1)
		return -1;

	return (RankInt * 4) + SuitInt + 1;

}

int HandEvaluator::GetCardInt(Card* _Card)
{
	int RankInt = static_cast<int>(_Card->GetRank());
	int SuitInt = static_cast<int>(_Card->GetSuit());

	return (RankInt * 4) + SuitInt + 1;
}

std::array<int,5> HandEvaluator::Get5CardsInt(std::array<Card*, 5> _Hand)
{
	std::array<int, 5> CardsInt;

	for (unsigned int Index = 0; Index < _Hand.size(); Index++)
		CardsInt[Index] = GetCardInt(_Hand[Index]);

	return CardsInt;
}

std::array<int,7> HandEvaluator::Get7CardsInt(std::array<Card*, 7> _Hand)
{
	std::array<int,7> CardsInt;

	for (unsigned int Index = 0; Index < _Hand.size(); Index++)
		CardsInt[Index] = GetCardInt(_Hand[Index]);
	
	return CardsInt;
}

double HandEvaluator::DetermineOdds_PreflopHole(std::array<Card*, 2> _Hole)
{
	int PairInt = GetCardInt(_Hole[0]) + GetCardInt(_Hole[1]);
	return PreflopOdds[PairInt];
}

double HandEvaluator::DetermineOdds_MonteCarlo(std::array<Card*, 2> _Hole, std::array<Card*, 5> _Community, int _TrialsAmt)
{
	unsigned int TrialsAmt = _TrialsAmt;
	unsigned int Win = 0, Lose = 0, Draw = 0, Count = 0;

	clock_t Clock = clock();

	for (auto TrialIndex = 0u; TrialIndex < TrialsAmt; TrialIndex++)
	{
		//Generate a random set of community cards and opponent's hole cards
		std::vector<Card*> Dead(_Hole.begin(), _Hole.end());
		//std::cout << "Dead Cards (Only Hole): " << GetStr(Dead) << std::endl;

		std::vector<Card*> Rand_Community(_Community.begin(), _Community.end());
		Rand_Community.erase(std::remove_if(Rand_Community.begin(), 
											Rand_Community.end(), 
											[](Card* _Card) {return _Card == nullptr; }), 
											Rand_Community.end());
		RandomFill(Rand_Community, Dead, 5);
		//std::cout << "Orignal Community Cards: " << GetStr(_Community) << "\n";
		//std::cout << "Random Community Cards: " << GetStr(Rand_Community) << "\n";

		Dead.insert(Dead.end(), Rand_Community.begin(), Rand_Community.end());
		//std::cout << "Dead Cards (Hole w/ Community): " << GetStr(Dead) << std::endl;

		std::vector<Card*> Rand_OppoHole;
		RandomFill(Rand_OppoHole, Dead, 2);
		//std::cout << "Random Opponent Hole Cards: " << GetStr(Rand_OppoHole) << std::endl;

		std::array<Card*, 7> PlayerHand;
		PlayerHand[0] = _Hole[0];
		PlayerHand[1] = _Hole[1];
		for (auto Index = 0u; Index < Rand_Community.size(); Index++)
			PlayerHand[Index + 2] = Rand_Community[Index];

		std::array<Card*, 7> OppoHand;
		OppoHand[0] = Rand_OppoHole[0];
		OppoHand[1] = Rand_OppoHole[1];
		for (auto Index = 0u; Index < Rand_Community.size(); Index++)
			OppoHand[Index + 2] = Rand_Community[Index];

		int PlayerVal = DetermineValue_7Cards(PlayerHand);
		int OppoVal	= DetermineValue_7Cards(OppoHand);

		if (PlayerVal > OppoVal)
			Win++;
		else if (PlayerVal < OppoVal)
			Lose++;
		else
			Draw++;

 		Count++;
	}

	double EstimatedOdds = (((double)Win) + ((double)Draw) / 2.0) / ((double)Count) * 100.0;
	
	//std::cout << "Winning Odds of " << GetStr(_Hole) << ": " << EstimatedOdds << "% - (" << TrialsAmt << " Trials in " << (clock() - Clock) * (1.0 / CLOCKS_PER_SEC) << " seconds) \n";
	return EstimatedOdds;
}

int HandEvaluator::DetermineValue_5Cards(std::array<Card*, 5> _Hand)
{
	std::array<int,5> CardInts = Get5CardsInt(_Hand); 
	
	int *Value1 = HR + HR[53 + CardInts[0]];
	int *Value2 = HR + Value1[CardInts[1]];
	int *Value3 = HR + Value2[CardInts[2]];
	int *Value4 = HR + Value3[CardInts[3]];
	int *Value5 = HR + Value4[CardInts[4]];

	return Value5[0]; 
}

int HandEvaluator::DetermineValue_7Cards(std::array<Card*, 7> _Hand)
{
	std::array<int,7> CardInts = Get7CardsInt(_Hand);

	int p = HR[53 + CardInts[0]];
	p = HR[p + CardInts[1]];
	p = HR[p + CardInts[2]];
	p = HR[p + CardInts[3]];
	p = HR[p + CardInts[4]];
	p = HR[p + CardInts[5]];

	return HR[p + CardInts[6]];
}

int HandEvaluator::DetermineValue_Custom(std::vector<Card*> _Hand)
{
	std::vector<int> CardInts;
	for (unsigned int Index = 0; Index < _Hand.size(); Index++)
		CardInts.push_back(GetCardInt(_Hand[Index]));

	int* Value = HR + HR[53 + CardInts[0]];
	for (unsigned int Index = 0; Index < CardInts.size(); Index++)
		Value = HR + Value[CardInts[Index]];

	return Value[0];
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

ComparisonResult HandEvaluator::IsBetter5Cards(std::array<Card*, 5> _First, std::array<Card*, 5> _Second)
{
	int FirstValue = DetermineValue_5Cards(_First);
	int SecondValue = DetermineValue_5Cards(_Second); 

	if (FirstValue == SecondValue)
		return ComparisonResult::Draw;

	return FirstValue > SecondValue ? ComparisonResult::Win : ComparisonResult::Lose;
}

ComparisonResult HandEvaluator::IsBetter7Cards(std::array<Card*, 7> _First, std::array<Card*, 7> _Second)
{
	int FirstValue = DetermineValue_7Cards(_First);
	int SecondValue = DetermineValue_7Cards(_Second);

	if (FirstValue == SecondValue)
		return ComparisonResult::Draw;

	return FirstValue > SecondValue ? ComparisonResult::Win : ComparisonResult::Lose;
}

ComparisonResult HandEvaluator::IsWorse5Cards(std::array<Card*, 5> _First, std::array<Card*, 5> _Second)
{
	return IsBetter5Cards(_Second, _First);
}

std::vector<Card*>::iterator HandEvaluator::UpperBound(std::vector<Card*>::iterator _First, std::vector<Card*>::iterator _Last, Card* _Current)
{
	for (auto Itr = _First; Itr != _Last; Itr++)
	{
		if ((*Itr)->GetRank() > _Current->GetRank())
			return Itr;
	}

	return _Last;
}

std::vector<Card*> HandEvaluator::InsertSort(std::vector<Card*> _Cards)
{
	std::vector<Card*>::iterator NextCard;
	std::vector<Card*>::iterator UpperCard;

	for (auto Itr = _Cards.begin(); Itr != _Cards.end(); Itr++)
	{
		NextCard = std::next(Itr);
		UpperCard = UpperBound(_Cards.begin(), Itr, *Itr);

		std::rotate(UpperCard, Itr, NextCard);
	}

	return _Cards;
}

unsigned int HandEvaluator::CountCardsWithValue(std::vector<Card*> _Cards, Rank _Value)
{
	unsigned int Count = 0;

	for (unsigned int Index = 0; Index < _Cards.size(); Index++)
	{
		if (_Cards[Index]->GetRank() == _Value)
			Count++;
	}

	return Count;
}

std::array<Card*, 5> HandEvaluator::SortHand(std::array<Card*, 5> _Hand)
{
	std::vector<Card*> SortingHand(_Hand.begin(), _Hand.end());

	//Determine if there are cards that share the same value in the hand
	std::vector<Card*> CardsWithMultipleCopies;

	for (unsigned int Index = 0; Index < 5; Index++)
	{
		if (CountCardsWithValue(SortingHand, SortingHand[Index]->GetRank()) > 1)
			CardsWithMultipleCopies.push_back(SortingHand[Index]);
	}

	//If there isn't any, perform generic insert sort on the hand will do
	if (CardsWithMultipleCopies.size() == 0)
	{
		SortingHand = InsertSort(SortingHand);

		//If this hand is a wheel straight (eg. A, 2, 3 ,4 ,5), the Ace card will be shifted to the first slot
		if (SortingHand[4]->GetRank() == Rank::Ace && SortingHand[0]->GetRank() == Rank::Two && SortingHand[1]->GetRank() == Rank::Three && SortingHand[2]->GetRank() == Rank::Four && SortingHand[3]->GetRank() == Rank::Five)
		{
			std::rotate(SortingHand.begin(), SortingHand.begin() + 4, SortingHand.end());
		}

		std::array<Card*, 5> SortedHand;
		std::copy_n(SortingHand.begin(), 5, SortedHand.begin());

		return SortedHand;
	}

	//If there are, sort those cards and the remaining cards in hand seperately
	//Store the remaining cards somewhere for sorting purpose
	std::vector<Card*> CardsWithSingleCopy;

	for (unsigned int Index = 0; Index < 5; Index++)
	{
		if (CountCardsWithValue(SortingHand, SortingHand[Index]->GetRank()) == 1)
			CardsWithSingleCopy.push_back(SortingHand[Index]);
	}

	//Sort both set of cards
	CardsWithMultipleCopies = InsertSort(CardsWithMultipleCopies);
	CardsWithSingleCopy = InsertSort(CardsWithSingleCopy);

	//Merge them back into a single hand
	SortingHand.clear();
	SortingHand.reserve(5);
	SortingHand.insert(SortingHand.end(), CardsWithMultipleCopies.begin(), CardsWithMultipleCopies.end());
	SortingHand.insert(SortingHand.end(), CardsWithSingleCopy.begin(), CardsWithSingleCopy.end());

	//If this hand is full-house, check if there is a need to swap the cards to get a proper order of 3 cards to 2 cards
	if (CountCardsWithValue(SortingHand, SortingHand[0]->GetRank()) == 2 && CountCardsWithValue(SortingHand, SortingHand[2]->GetRank()) == 3)
	{
		Card* ShiftingCards[2] = { SortingHand[0], SortingHand[1] };

		//Remove the first two cards from front
		SortingHand.erase(SortingHand.begin());
		SortingHand.erase(SortingHand.begin());

		//Add them to the back
		SortingHand.insert(SortingHand.end(), ShiftingCards[0]);
		SortingHand.insert(SortingHand.end(), ShiftingCards[1]);
	}

	std::array<Card*, 5> SortedHand;
	std::copy_n(SortingHand.begin(), 5, SortedHand.begin());

	return SortedHand;
}

std::vector<Card*> HandEvaluator::GetDeckComposition()
{
	std::vector<Card*> Cards;

	for (unsigned int SIndex = 0; SIndex < 4; SIndex++)
	{
		for (unsigned int VIndex = 0; VIndex < 13; VIndex++)
		{
			Suit NewSuit = static_cast<Suit>(SIndex);
			Rank NewValue = static_cast<Rank>(VIndex);

			Cards.push_back(new Card(NewSuit, NewValue));
		}
	}

	return Cards;
}

std::array<Card*, 5> HandEvaluator::GetBestCommunalHand(std::array<Card*, 2> _Hole, std::array<Card*, 5> _Communal)
{
	std::array<Card*, 5> BestHand;

	if (_Communal[3] == nullptr)
	{
		BestHand[0] = _Communal[0];
		BestHand[1] = _Communal[1];
		BestHand[2] = _Communal[2];
		BestHand[3] = _Hole[0];
		BestHand[4] = _Hole[1];
		return BestHand;
	}
	else if (_Communal[4] == nullptr)
	{
		std::vector<std::array<Card*, 5>> PossibleHands;
		PossibleHands.push_back(*new std::array<Card*, 5>{_Communal[0], _Communal[1], _Communal[2], _Communal[3], _Hole[0]});
		PossibleHands.push_back(*new std::array<Card*, 5>{_Communal[0], _Communal[1], _Communal[2], _Communal[3], _Hole[1]});
		PossibleHands.push_back(*new std::array<Card*, 5>{_Communal[0], _Communal[1], _Communal[2], _Hole[0], _Hole[1]});
		PossibleHands.push_back(*new std::array<Card*, 5>{_Communal[0], _Communal[1], _Communal[3], _Hole[0], _Hole[1]});
		PossibleHands.push_back(*new std::array<Card*, 5>{_Communal[0], _Communal[2], _Communal[3], _Hole[0], _Hole[1]});
		PossibleHands.push_back(*new std::array<Card*, 5>{_Communal[1], _Communal[2], _Communal[3], _Hole[0], _Hole[1]});

		BestHand = PossibleHands[0];

		for (unsigned int Index = 1; Index < PossibleHands.size(); Index++)
		{
			if (IsBetter5Cards(PossibleHands[Index], BestHand) == ComparisonResult::Win)
				BestHand = PossibleHands[Index];
		}

		return BestHand;
	}

	//5 community cards
	std::vector<std::array<Card*, 5>> PossibleHands;
	BestHand = _Communal;

	//Combination with first or second hole card
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Communal[0], _Communal[2], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Communal[1], _Communal[2], _Communal[3], _Communal[4] });

	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[1], _Communal[0], _Communal[2], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[1], _Communal[1], _Communal[2], _Communal[3], _Communal[4] });

	//Combination with both hole cards	 
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[1], _Communal[2] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[1], _Communal[3] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[1], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[1], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[1], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[1], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[2], _Communal[3], _Communal[4] });

	for (unsigned int Index = 0; Index < PossibleHands.size(); Index++)
	{
		if (IsBetter5Cards(PossibleHands[Index], BestHand) == ComparisonResult::Win)
			BestHand = PossibleHands[Index];
	}

	return BestHand;
}

std::array<Card*, 5> HandEvaluator::GetWorstCommunalHand(std::array<Card*, 2> _Hole, std::array<Card*, 5> _Communal)
{
	//5 community cards
	std::vector<std::array<Card*, 5>> PossibleHands;
	std::array<Card*, 5> WorstHand = _Communal;

	//Combination with first or second hole card
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Communal[0], _Communal[2], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Communal[1], _Communal[2], _Communal[3], _Communal[4] });

	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[1], _Communal[0], _Communal[2], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[1], _Communal[1], _Communal[2], _Communal[3], _Communal[4] });

	//Combination with both hole cards	 
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[1], _Communal[2] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[1], _Communal[3] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[1], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[1], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[1], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[1], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<Card*, 5>{ _Hole[0], _Hole[1], _Communal[2], _Communal[3], _Communal[4] });

	for (unsigned int Index = 0; Index < PossibleHands.size(); Index++)
	{
		if (IsWorse5Cards(PossibleHands[Index], WorstHand) == ComparisonResult::Win)
			WorstHand = PossibleHands[Index];
	}

	return WorstHand;
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

std::string HandEvaluator::GetTypeStr(std::array<Card*, 5> _Hand)
{
	int Value = DetermineValue_5Cards(_Hand);
	Hand Type = DetermineType(Value);

	return GetTypeStr(Type);
}

std::string HandEvaluator::GetStr(std::array<Card*, 2> _Hole)
{
	if (_Hole[0] == nullptr)
		return "";

	else if (_Hole[1] == nullptr)
		return _Hole[0]->GetInfo();

	return _Hole[0]->GetInfo() + " " + _Hole[1]->GetInfo();
}

std::string HandEvaluator::GetStr(std::array<Card*, 5> _Hand)
{
	std::string str = "";

	for (auto Index = 0u; Index < _Hand.size(); Index++)
	{
		if (_Hand[Index] == nullptr)
			return str;

		str += _Hand[Index]->GetInfo() + " ";
	}

	return str;
}

std::string HandEvaluator::GetStr(std::array<Card*, 7> _Hand)
{
	std::string str = "";

	for (auto Index = 0u; Index < _Hand.size(); Index++)
	{
		if (_Hand[Index] == nullptr)
			return str;

		str += _Hand[Index]->GetInfo() + " ";
	}

	return str;

	//return _Hand[0]->GetInfo() + "/" + _Hand[1]->GetInfo() + "/" + _Hand[2]->GetInfo() + "/" + _Hand[3]->GetInfo() + "/" + _Hand[4]->GetInfo() + "/" + _Hand[5]->GetInfo() + "/" + _Hand[6]->GetInfo();
}

std::string HandEvaluator::GetStr(std::vector<Card*> _Hand)
{
	std::string str = "";

	for (auto Index = 0u; Index < _Hand.size(); Index++)
	{
		if (_Hand[Index] == nullptr)
			return str;

		str += _Hand[Index]->GetInfo() + " ";
	}

	return str;

	/*if (_Hand.empty() || _Hand[0] == nullptr)
		return "";

	std::string Str = "";

	for (unsigned int Index = 0; Index < _Hand.size(); Index++)
	{
		if (Index != 0)
			Str += "/";

		Str += _Hand[Index]->GetInfo();
	}

	return Str;*/
}