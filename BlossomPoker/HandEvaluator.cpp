#include "HandEvaluator.h"
#include "Card.h"
#include "xoroshiro128+.h"

HandEvaluator::HandEvaluator()
{
	Initialize();

	//MTGenerator.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	s[0] = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	s[1] = std::chrono::high_resolution_clock::now().time_since_epoch().count();
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

	for (auto const& OddData : Data)
		outfile << OddData[0] << " " << OddData[1] << std::endl;

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
	fclose(fin);

	int ReferenceIndex = 0;

	for (unsigned int SIndex = 0; SIndex < 4; SIndex++)
	{
		for (unsigned int VIndex = 0; VIndex < 13; VIndex++)
		{
			ReferenceDeck[ReferenceIndex] = std::make_shared<Card>(static_cast<Suit>(SIndex), static_cast<Rank>(VIndex));
			ReferenceIndex++;
		}
	}
}

void HandEvaluator::RandomFill_Array(std::array<std::shared_ptr<Card>, 7>& _Set, std::vector<std::shared_ptr<Card>>& _Dead, unsigned int _Index, unsigned int _Amt)
{
	//Add the cards that are currently in Set as dead cards
	for (auto const& CardInSet : _Set)
	{
		if (CardInSet == nullptr)
			break;

		_Dead.push_back(CardInSet);
	}

	bool IsDead;

	for (unsigned int Index = _Index; Index < _Index + _Amt; Index++)
	{
		while (true)
		{
			_Set[Index] = ReferenceDeck[next() % 52];

			IsDead = false;

			for (auto const& Dead : _Dead)
			{
				if (Dead->IsEqualTo(_Set[Index]))
				{
					IsDead = true;
					break;
				}
			}

			if (IsDead)
				_Set[Index] = nullptr;
			else
				break;
		}
	}
}

void HandEvaluator::RandomFill_Vector(std::vector<std::shared_ptr<Card>>& _Set, std::vector<std::shared_ptr<Card>>& _Dead, unsigned int _Target)
{
	//Add the cards that are currently in Set as dead cards
	for (auto const& CardInSet : _Set)
	{
		if (CardInSet == nullptr)
			break;

		_Dead.push_back(CardInSet);
	}

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
				if (Dead->IsEqualTo(_Set[_Set.size() - 1]))
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

int HandEvaluator::GetCardInt(const std::shared_ptr<Card>& _Card)
{
	return (_Card->GetRankInt() * 4) + _Card->GetSuitInt() + 1;
}

void HandEvaluator::Get5CardsInt(const std::array<std::shared_ptr<Card>, 5>& _Hand, std::array<int,5>& _CardInts)
{
	for (unsigned int Index = 0; Index < _Hand.size(); Index++)
		_CardInts[Index] = GetCardInt(_Hand[Index]);
}

void HandEvaluator::Get7CardsInt(const std::vector<std::shared_ptr<Card>>& _Hand, std::array<int, 7>& _CardInts)
{
	for (unsigned int Index = 0; Index < _Hand.size(); Index++)
		_CardInts[Index] = GetCardInt(_Hand[Index]);
}

void HandEvaluator::Get7CardsInt(const std::array<std::shared_ptr<Card>, 7>& _Hand, std::array<int,7>& _CardInts)
{
	for (unsigned int Index = 0; Index < _Hand.size(); Index++)
		_CardInts[Index] = GetCardInt(_Hand[Index]);
}

float HandEvaluator::DetermineOdds_PreflopHole(std::array<std::shared_ptr<Card>, 2> _Hole)
{
	return PreflopOdds[GetCardInt(_Hole[0]) + GetCardInt(_Hole[1])];
}

float HandEvaluator::DetermineOdds_MonteCarlo(std::array<std::shared_ptr<Card>, 2> _Hole, std::array<std::shared_ptr<Card>	, 5> _Community, unsigned int _TrialsAmt)
{
	int PlayerVal, OppoVal;
	unsigned int Win = 0, Draw = 0, GameCount = 0, ExistingCount = 0;

	std::array<std::shared_ptr<Card>, 7> PlayerHand;
	std::array<std::shared_ptr<Card>, 7> OppoHand;

	std::vector<std::shared_ptr<Card>> Dead;
	std::vector<std::shared_ptr<Card>> Rand_Community;
	std::vector<std::shared_ptr<Card>> Rand_OppoHole;

	//Insert known information (Player's hole cards and current Community cards)
	for (auto const& Card : _Community)
	{
		if (Card != nullptr)
		{
			Rand_Community.push_back(Card);
			ExistingCount++;
		}
	}

	PlayerHand[0] = _Hole[0];
	PlayerHand[1] = _Hole[1];
	for (unsigned int Index = 0; Index < ExistingCount; Index++)
	{
		PlayerHand[2 + Index] = Rand_Community[Index];
		OppoHand[Index] = Rand_Community[Index];
	}

	Dead.insert(Dead.end(), _Hole.begin(), _Hole.end());
	Dead.insert(Dead.end(), Rand_Community.begin(), Rand_Community.end());

	for (unsigned int TrialIndex = 0; TrialIndex < _TrialsAmt; TrialIndex++)
	{
		//Add random cards (excluding dead cards) into Community until it reached 5 cards
		RandomFill_Vector(Rand_Community, Dead, 5);

		Dead.insert(Dead.end(), Rand_Community.begin() + ExistingCount, Rand_Community.end());

		//Randomly get Opponent Hand based on current Dead Cards 
		RandomFill_Vector(Rand_OppoHole, Dead, 2);

		//Add communal cards into both player's and opponent's hand
		for (unsigned int Index = ExistingCount, Maximum = Rand_Community.size(); Index < Maximum; Index++)
			PlayerHand[Index + 2] = Rand_Community[Index];

		OppoHand[ExistingCount] = Rand_OppoHole[0];
		OppoHand[ExistingCount + 1] = Rand_OppoHole[1];
		for (unsigned int Index = ExistingCount, Maximum = Rand_Community.size(); Index < Maximum; Index++)
			OppoHand[Index + 2] = Rand_Community[Index];
		
		//Get scores for both hands to determine winner
		PlayerVal = DetermineValue_7Cards(PlayerHand);
		OppoVal = DetermineValue_7Cards(OppoHand);

		if (PlayerVal > OppoVal)
			Win++;
		else if(PlayerVal == OppoVal)
			Draw++;

		GameCount++;

		//Erase all random info
		Dead.erase(Dead.begin() + ExistingCount, Dead.end()); 
		Rand_Community.erase(Rand_Community.begin() + ExistingCount, Rand_Community.end());
		Rand_OppoHole.clear();
	}

	return (((float)Win) + ((float)Draw) / 2.0f) / ((float)GameCount) * 100.0f;
}

float HandEvaluator::DetermineOdds_MonteCarlo_Multi(std::array<std::shared_ptr<Card>, 2> _Hole, std::array<std::shared_ptr<Card>, 5> _Community, unsigned int _PlayerAmt, unsigned int _TrialsAmt)
{
	std::vector<std::shared_ptr<Card>> Hand_Player;
	std::vector<std::vector<std::shared_ptr<Card>>> Hand_Opponents;
	std::vector<std::shared_ptr<Card>> Community;
	std::vector<std::shared_ptr<Card>> Dead;

	unsigned int Score_Player = 0;
	std::vector<unsigned int> Score_Opponents;

	for (unsigned int Index = 0; Index < _PlayerAmt - 1; Index++)
	{
		Score_Opponents.push_back(0);
		Hand_Opponents.push_back({ });
	}

	bool HasPlayerWin = true;

	unsigned int Win = 0, Draw = 0, GameCount = 0;
	unsigned int ExistingCount = 0;

	//Add Player's Hole Cards
	Hand_Player.insert(Hand_Player.end(), _Hole.begin(), _Hole.end());
	Dead.insert(Dead.end(), _Hole.begin(), _Hole.end());

	//Add existing Communal Cards
	for (auto const& Card : _Community)
	{
		if (Card != nullptr)
		{
			Community.push_back(Card);
			ExistingCount++;
		}
	}
	Hand_Player.insert(Hand_Player.end(), Community.begin(), Community.end());
	for (auto& Hand_Opponent : Hand_Opponents)
		Hand_Opponent.insert(Hand_Opponent.end(), Community.begin(), Community.end());
	Dead.insert(Dead.end(), Community.begin(), Community.end());

	for (unsigned int TrialIndex = 0; TrialIndex < _TrialsAmt; TrialIndex++)
	{
		//Generate Opponent's Hole Cards
		for (auto& Hand_Opponent : Hand_Opponents)
			RandomFill_Vector(Hand_Opponent, Dead, Hand_Opponent.size() + 2);

		//Generate remaining Communal Cards
		RandomFill_Vector(Community, Dead, 5);

		//Add remaining Communal Cards to Player and Opponents
		Hand_Player.insert(Hand_Player.end(), Community.begin() + ExistingCount, Community.end());
		for (auto& Hand_Opponent : Hand_Opponents)
			Hand_Opponent.insert(Hand_Opponent.end(), Community.begin() + ExistingCount, Community.end());

		//Determine score for Player's and Oppponent's Hand
		Score_Player = DetermineValue_7Cards(Hand_Player);
		for (unsigned int Index = 0; Index < _PlayerAmt - 1; Index++)
			Score_Opponents[Index] = DetermineValue_7Cards(Hand_Opponents[Index]);

		HasPlayerWin = true;
		for (auto &Score_Opponent : Score_Opponents)
		{
			if (Score_Player < Score_Opponent)
			{
				HasPlayerWin = false;
				break;
			}
		}

		if (HasPlayerWin)
			Win++;

		GameCount++;

		Dead.erase(Dead.begin() + ExistingCount, Dead.end());
		Community.erase(Community.begin() + ExistingCount, Community.end());

		Hand_Player.erase(Hand_Player.begin() + (2 + ExistingCount), Hand_Player.end());
		for (auto& Hand_Opponent : Hand_Opponents)
			Hand_Opponent.erase(Hand_Opponent.begin() + ExistingCount, Hand_Opponent.end());
	}

	return (((float)Win) + ((float)Draw) / 2.0f) / ((float)GameCount) * 100.0f;
}

int HandEvaluator::DetermineValue_5Cards(const std::array<std::shared_ptr<Card>, 5>& _Hand)
{
	std::array<int, 5> CardInts;
	Get5CardsInt(_Hand,CardInts);
	
	int *Value1 = HR + HR[53 + CardInts[0]];
	int *Value2 = HR + Value1[CardInts[1]];
	int *Value3 = HR + Value2[CardInts[2]];
	int *Value4 = HR + Value3[CardInts[3]];
	int *Value5 = HR + Value4[CardInts[4]];

	return Value5[0]; 
}

int HandEvaluator::DetermineValue_7Cards(const std::vector<std::shared_ptr<Card>>& _Hand)
{
	std::array<int, 7> CardInts;
	Get7CardsInt(_Hand, CardInts);

	return HR[HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]];
}

int HandEvaluator::DetermineValue_7Cards(const std::array<std::shared_ptr<Card>, 7>& _Hand)
{
	std::array<int, 7> CardInts;
	Get7CardsInt(_Hand,CardInts);

	/*int p = HR[53 + CardInts[0]];
	p = HR[p + CardInts[1]];
	p = HR[p + CardInts[2]];
	p = HR[p + CardInts[3]];
	p = HR[p + CardInts[4]];
	p = HR[p + CardInts[5]];*/

	return HR[HR[HR[HR[HR[HR[53 + CardInts[0]] + CardInts[1]] + CardInts[2]] + CardInts[3]] + CardInts[4]] + CardInts[5]];
}

int HandEvaluator::DetermineValue_Custom(const std::vector<std::shared_ptr<Card>>& _Hand)
{
	std::vector<int> CardInts;
	for (auto& Card : _Hand)
		CardInts.push_back(GetCardInt(Card));

	int* Value = HR + HR[53 + CardInts[0]];
	for (auto& CardInt : CardInts)
		Value = HR + Value[CardInt];

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

ComparisonResult HandEvaluator::IsBetter5Cards(std::array<std::shared_ptr<Card>, 5> _First, std::array<std::shared_ptr<Card>, 5> _Second)
{
	int FirstValue = DetermineValue_5Cards(_First);
	int SecondValue = DetermineValue_5Cards(_Second); 

	if (FirstValue == SecondValue)
		return ComparisonResult::Draw;

	return FirstValue > SecondValue ? ComparisonResult::Win : ComparisonResult::Lose;
}

ComparisonResult HandEvaluator::IsBetter7Cards(std::array<std::shared_ptr<Card>, 7> _First, std::array<std::shared_ptr<Card>, 7> _Second)
{
	int FirstValue = DetermineValue_7Cards(_First);
	int SecondValue = DetermineValue_7Cards(_Second);

	if (FirstValue == SecondValue)
		return ComparisonResult::Draw;

	return FirstValue > SecondValue ? ComparisonResult::Win : ComparisonResult::Lose;
}

ComparisonResult HandEvaluator::IsWorse5Cards(std::array<std::shared_ptr<Card>, 5> _First, std::array<std::shared_ptr<Card>, 5> _Second)
{
	return IsBetter5Cards(_Second, _First);
}

std::vector<std::shared_ptr<Card>>::iterator HandEvaluator::UpperBound(std::vector<std::shared_ptr<Card>>::iterator _First, std::vector<std::shared_ptr<Card>>::iterator _Last, std::shared_ptr<Card> _Current)
{
	for (auto Itr = _First; Itr != _Last; Itr++)
	{
		if ((*Itr)->GetRank() > _Current->GetRank())
			return Itr;
	}

	return _Last;
}

void HandEvaluator::InsertSort(std::vector<std::shared_ptr<Card>>& _Cards)
{
	std::vector<std::shared_ptr<Card>>::iterator NextCard;
	std::vector<std::shared_ptr<Card>>::iterator UpperCard;

	for (auto Itr = _Cards.begin(); Itr != _Cards.end(); Itr++)
	{
		NextCard = std::next(Itr);
		UpperCard = UpperBound(_Cards.begin(), Itr, *Itr);

		std::rotate(UpperCard, Itr, NextCard);
	}
}

unsigned int HandEvaluator::CountCardsWithValue(std::vector<std::shared_ptr<Card>> _Cards, Rank _Value)
{
	unsigned int Count = 0;

	for (auto const& Card : _Cards)
	{
		if (Card->GetRank() == _Value)
			Count++;
	}

	return Count;
}

std::array<std::shared_ptr<Card>, 5> HandEvaluator::SortHand(std::array<std::shared_ptr<Card>, 5> _Hand)
{
	std::vector<std::shared_ptr<Card>> SortingHand(_Hand.begin(), _Hand.end());

	//Determine if there are cards that share the same value in the hand
	std::vector<std::shared_ptr<Card>> CardsWithMultipleCopies;

	for (unsigned int Index = 0; Index < 5; Index++)
	{
		if (CountCardsWithValue(SortingHand, SortingHand[Index]->GetRank()) > 1)
			CardsWithMultipleCopies.push_back(SortingHand[Index]);
	}

	//If there isn't any, perform generic insert sort on the hand will do
	if (CardsWithMultipleCopies.size() == 0)
	{
		InsertSort(SortingHand);

		//If this hand is a wheel straight (eg. A, 2, 3 ,4 ,5), the Ace card will be shifted to the first slot
		if (SortingHand[4]->GetRank() == Rank::Ace && SortingHand[0]->GetRank() == Rank::Two && SortingHand[1]->GetRank() == Rank::Three && SortingHand[2]->GetRank() == Rank::Four && SortingHand[3]->GetRank() == Rank::Five)
		{
			std::rotate(SortingHand.begin(), SortingHand.begin() + 4, SortingHand.end());
		}

		std::array<std::shared_ptr<Card>, 5> SortedHand;
		std::copy_n(SortingHand.begin(), 5, SortedHand.begin());

		return SortedHand;
	}

	//If there are, sort those cards and the remaining cards in hand seperately
	//Store the remaining cards somewhere for sorting purpose
	std::vector<std::shared_ptr<Card>> CardsWithSingleCopy;

	for (unsigned int Index = 0; Index < 5; Index++)
	{
		if (CountCardsWithValue(SortingHand, SortingHand[Index]->GetRank()) == 1)
			CardsWithSingleCopy.push_back(SortingHand[Index]);
	}

	//Sort both set of cards
	InsertSort(CardsWithMultipleCopies);
	InsertSort(CardsWithSingleCopy);

	//Merge them back into a single hand
	SortingHand.clear();
	SortingHand.reserve(5);
	SortingHand.insert(SortingHand.end(), CardsWithMultipleCopies.begin(), CardsWithMultipleCopies.end());
	SortingHand.insert(SortingHand.end(), CardsWithSingleCopy.begin(), CardsWithSingleCopy.end());

	//If this hand is full-house, check if there is a need to swap the cards to get a proper order of 3 cards to 2 cards
	if (CountCardsWithValue(SortingHand, SortingHand[0]->GetRank()) == 2 && CountCardsWithValue(SortingHand, SortingHand[2]->GetRank()) == 3)
	{
		std::shared_ptr<Card> ShiftingCards[2] = { SortingHand[0], SortingHand[1] };

		//Remove the first two cards from front
		SortingHand.erase(SortingHand.begin());
		SortingHand.erase(SortingHand.begin());

		//Add them to the back
		SortingHand.insert(SortingHand.end(), ShiftingCards[0]);
		SortingHand.insert(SortingHand.end(), ShiftingCards[1]);
	}

	std::array<std::shared_ptr<Card>, 5> SortedHand;
	std::copy_n(SortingHand.begin(), 5, SortedHand.begin());

	return SortedHand;
}

std::array<std::shared_ptr<Card>, 5> HandEvaluator::GetBestCommunalHand(std::array<std::shared_ptr<Card>, 2> _Hole, std::array<std::shared_ptr<Card>, 5> _Communal)
{
	std::array<std::shared_ptr<Card>, 5> BestHand;

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
		std::vector<std::array<std::shared_ptr<Card>, 5>> PossibleHands;
		PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{_Communal[0], _Communal[1], _Communal[2], _Communal[3], _Hole[0]});
		PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{_Communal[0], _Communal[1], _Communal[2], _Communal[3], _Hole[1]});
		PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{_Communal[0], _Communal[1], _Communal[2], _Hole[0], _Hole[1]});
		PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{_Communal[0], _Communal[1], _Communal[3], _Hole[0], _Hole[1]});
		PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{_Communal[0], _Communal[2], _Communal[3], _Hole[0], _Hole[1]});
		PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{_Communal[1], _Communal[2], _Communal[3], _Hole[0], _Hole[1]});

		BestHand = PossibleHands[0];

		for (auto const& Hand : PossibleHands)
		{
			if (IsBetter5Cards(Hand, BestHand) == ComparisonResult::Win)
				BestHand = Hand;
		}

		return BestHand;
	}

	//5 community cards
	std::vector<std::array<std::shared_ptr<Card>, 5>> PossibleHands;
	BestHand = _Communal;

	//Combination with first or second hole card
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Communal[0], _Communal[2], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Communal[1], _Communal[2], _Communal[3], _Communal[4] });

	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[1], _Communal[0], _Communal[2], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[1], _Communal[1], _Communal[2], _Communal[3], _Communal[4] });

	//Combination with both hole cards	 
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[1], _Communal[2] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[1], _Communal[3] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[1], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[1], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[1], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[1], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[2], _Communal[3], _Communal[4] });

	for (auto const& Hand : PossibleHands)
	{
		if (IsBetter5Cards(Hand, BestHand) == ComparisonResult::Win)
			BestHand = Hand;
	}

	return BestHand;
}

std::array<std::shared_ptr<Card>, 5> HandEvaluator::GetWorstCommunalHand(std::array<std::shared_ptr<Card>, 2> _Hole, std::array<std::shared_ptr<Card>, 5> _Communal)
{
	//5 community cards
	std::vector<std::array<std::shared_ptr<Card>, 5>> PossibleHands;
	std::array<std::shared_ptr<Card>, 5> WorstHand = _Communal;

	//Combination with first or second hole card
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Communal[0], _Communal[2], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Communal[1], _Communal[2], _Communal[3], _Communal[4] });

	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[1], _Communal[0], _Communal[2], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[1], _Communal[1], _Communal[2], _Communal[3], _Communal[4] });

	//Combination with both hole cards	 
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[1], _Communal[2] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[1], _Communal[3] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[1], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[0], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[1], _Communal[2], _Communal[3] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[1], _Communal[2], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[1], _Communal[3], _Communal[4] });
	PossibleHands.push_back(*new std::array<std::shared_ptr<Card>, 5>{ _Hole[0], _Hole[1], _Communal[2], _Communal[3], _Communal[4] });

	for (auto const& Hand : PossibleHands)
	{
		if (IsWorse5Cards(Hand, WorstHand) == ComparisonResult::Win)
			WorstHand = Hand;
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

std::string HandEvaluator::GetTypeStr(std::array<std::shared_ptr<Card>, 5> _Hand)
{
	return GetTypeStr(DetermineType(DetermineValue_5Cards(_Hand)));
}

std::string HandEvaluator::GetStr(std::array<std::shared_ptr<Card>, 2> _Hole)
{
	if (_Hole[0] == nullptr)
		return "";

	else if (_Hole[1] == nullptr)
		return _Hole[0]->GetInfo();

	return _Hole[0]->GetInfo() + " " + _Hole[1]->GetInfo();
}

std::string HandEvaluator::GetStr(std::array<std::shared_ptr<Card>, 5> _Hand)
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

std::string HandEvaluator::GetStr(std::array<std::shared_ptr<Card>, 7> _Hand)
{
	std::string str = "";

	for (auto const& Card : _Hand)
	{
		if (Card == nullptr)
			return str;

		str += Card->GetInfo() + " ";
	}

	return str;
}

std::string HandEvaluator::GetStr(std::vector<std::shared_ptr<Card>> _Hand)
{
	std::string str = "";

	for (auto const& Card : _Hand)
	{
		if (Card == nullptr)
			return str;

		str += Card->GetInfo() + " ";
	}

	return str;
}