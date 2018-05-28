#include "HandsEvaluator.h"
#include <algorithm>
#include <iterator>
#include <iostream>

namespace HandsEvaluator
{
	Card* HandsEvaluator::LowestReference()
	{
		return new Card(Suit::Diamond, Value::Two);
	}

	Card* HandsEvaluator::HighestReference()
	{
		return new Card(Suit::Spade, Value::Ace);
	}

	std::vector<Card*>::iterator HandsEvaluator::UpperBound(std::vector<Card*>::iterator _First, std::vector<Card*>::iterator _Last, Card* _Current)
	{
		for (auto Itr = _First; Itr != _Last; Itr++)
		{
			if ((*Itr)->GetValueInt() > _Current->GetValueInt())
				return Itr;
		}

		return _Last;
	}
	
	std::vector<Card*> HandsEvaluator::InsertSort(std::vector<Card*> _Cards)
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

	std::array<Card*,5> HandsEvaluator::SortHand(std::array<Card*, 5> _Hand)
	{
		std::vector<Card*> SortingHand(_Hand.begin(), _Hand.end());

		//Determine if there are cards that share the same value in the hand
		std::vector<Card*> CardsWithMultipleCopies;
		
		for (unsigned int Index = 0; Index < 5; Index++)
		{
			if (CountCardsWithValue(SortingHand, SortingHand[Index]->GetValue()) > 1)
				CardsWithMultipleCopies.push_back(SortingHand[Index]);
		}

		//If there isn't any, perform generic insert sort on the hand will do
		if (CardsWithMultipleCopies.size() == 0)
		{
			SortingHand = InsertSort(SortingHand);

			//If this hand is a wheel straight (eg. A, 2, 3 ,4 ,5), the Ace card will be shifted to the first slot
			if (SortingHand[4]->GetValue() == Value::Ace && SortingHand[0]->GetValue() == Value::Two && SortingHand[1]->GetValue() == Value::Three && SortingHand[2]->GetValue() == Value::Four && SortingHand[3]->GetValue() == Value::Five)
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
			if (CountCardsWithValue(SortingHand, SortingHand[Index]->GetValue()) == 1)
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
		if (CountCardsWithValue(SortingHand, SortingHand[0]->GetValue()) == 2 && CountCardsWithValue(SortingHand, SortingHand[2]->GetValue()) == 3)
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

	bool HandsEvaluator::IsFlush(std::vector<Card*> _Hand)
	{
		bool IsFlushType = true;

		for (unsigned int Index = 0; Index < 5; Index++)
			if (_Hand[Index]->GetSuit() != _Hand[0]->GetSuit()) return false;

		return true;
	}

	bool HandsEvaluator::IsStraight(std::vector<Card*> _Hand)
	{
		int FirstValue = static_cast<int>(_Hand[0]->GetValue());
		int SecondValue = static_cast<int>(_Hand[1]->GetValue());
		int ThirdValue = static_cast<int>(_Hand[2]->GetValue());
		int FouthValue = static_cast<int>(_Hand[3]->GetValue());
		int FifthValue = static_cast<int>(_Hand[4]->GetValue());

		if (FifthValue == FouthValue + 1 && FouthValue == ThirdValue + 1 && ThirdValue == SecondValue + 1 && SecondValue == FirstValue + 1)
			return true;
		
		else if (_Hand[0]->GetValue() == Value::Ace && _Hand[1]->GetValue() == Value::Two && ThirdValue == SecondValue + 1 && FouthValue == ThirdValue + 1)
			return true;

		return false;
	}

	unsigned int HandsEvaluator::CountCardsWithValue(std::vector<Card*> _Hand, Value _Value)
	{
		unsigned int Count = 0;

		for (unsigned int Index = 0; Index < 5; Index++)
			if (_Hand[Index]->GetValue() == _Value) Count++;

		return Count;
	}

	Hand HandsEvaluator::DetermineHandType(std::array<Card*, 5> _Hand)
	{
		std::vector<Card*> CurrentHand(_Hand.begin(), _Hand.end());
		return DetermineHandType(CurrentHand);
	}

	Hand HandsEvaluator::DetermineHandType(std::vector<Card*> _Hand)
	{
		if (IsFlush(_Hand) && IsStraight(_Hand))
			return _Hand[0]->GetValue() == Value::Ten ? Hand::RoyalFlush : Hand::StraightFlush;

		if (CountCardsWithValue(_Hand, _Hand[0]->GetValue()) == 4)
			return Hand::FourKind;

		if (CountCardsWithValue(_Hand, _Hand[0]->GetValue()) == 3 && CountCardsWithValue(_Hand, _Hand[3]->GetValue()) == 2)
			return Hand::FullHouse;

		if (IsFlush(_Hand))
			return Hand::Flush;

		if (IsStraight(_Hand))
			return Hand::Straight;

		if (CountCardsWithValue(_Hand, _Hand[0]->GetValue()) == 3)
			return Hand::ThreeKind;

		if (CountCardsWithValue(_Hand, _Hand[0]->GetValue()) == 2 && CountCardsWithValue(_Hand, _Hand[2]->GetValue()) == 2)
			return Hand::TwoPair;

		if (CountCardsWithValue(_Hand, _Hand[0]->GetValue()) == 2)
			return Hand::Pair;

		return Hand::High;
	}

	ComparisonResult HandsEvaluator::CompareHand(std::array<Card*, 5> _First, std::array<Card*,5> _Second)
	{
		Hand FirstType = DetermineHandType(_First);
		Hand SecondType = DetermineHandType(_Second);

		if (FirstType != SecondType)
			return FirstType > SecondType ? ComparisonResult::Win : ComparisonResult::Lose;
		
		if (FirstType == Hand::High)
		{
			//Loop from the back of both hands, compare each pair of cards to determine hand superiority
			//Loop from (5 - 1) to 0
			for (unsigned int Index = 5; Index-- > 0;)
			{
				if (_First[Index]->GetValue() != _Second[Index]->GetValue())
					return _First[Index]->GetValueInt() > _Second[Index]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;
			}

			return ComparisonResult::Draw;
		}
		
		else if (FirstType == Hand::Pair)
		{
			//Compare the pairs of both hands for hand superiority
			Card* First_Pair = _First[0];
			Card* Second_Pair = _Second[0];

			if (First_Pair->GetValue() != Second_Pair->GetValue())
				return First_Pair->GetValueInt() > Second_Pair->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;

			//If the pairs are of same value, compare the kicker cards instead
			for (unsigned int Index = 4; Index >= 2; Index--)
			{
				if (_First[Index]->GetValue() != _Second[Index]->GetValue())
					return _First[Index]->GetValueInt() > _Second[Index]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;
			}

			return ComparisonResult::Draw;
		}

		else if (FirstType == Hand::TwoPair)
		{
			//Compare the first pair of both hands
			if (_First[0]->GetValue() != _Second[0]->GetValue())
				return _First[0]->GetValueInt() > _Second[0]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;

			//Compare the second pair of both hands
 			if (_First[2]->GetValue() != _Second[2]->GetValue())
				return _First[2]->GetValueInt() > _Second[2]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;

			//Compare the kicker cards of both hands
			if (_First[4]->GetValue() != _Second[4]->GetValue())
				return _First[4]->GetValueInt() > _Second[4]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;

			return ComparisonResult::Draw;
		}

		else if (FirstType == Hand::ThreeKind)
		{
			//Compare the 3-cards of both hands
			if (_First[0]->GetValue() != _Second[0]->GetValue())
				return _First[0]->GetValueInt() > _Second[0]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;

			//Loop through the two kickers of both hands and compare them
			for (unsigned int Index = 4; Index >= 3; Index--)
			{
				if (_First[Index]->GetValue() != _Second[Index]->GetValue())
					return _First[Index]->GetValueInt() > _Second[Index]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;
			}

			return ComparisonResult::Draw;
		}

		else if (FirstType == Hand::Straight)
		{
			//Compare the last card AKA largest value of both hands
			if (_First[4]->GetValue() != _Second[4]->GetValue())
				return _First[4]->GetValueInt() > _Second[4]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;
		
			return ComparisonResult::Draw;
		}
		
		else if (FirstType == Hand::Flush)
		{
			//Loop through the cards from the back of both hands and compare each pair of cards for hand superiority
			for (unsigned int Index = 5; Index-- > 0;)
			{
				if (_First[Index]->GetValue() != _Second[Index]->GetValue())
					return _First[Index]->GetValueInt() > _Second[Index]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;
			}

			return ComparisonResult::Draw;
		}

		else if (FirstType == Hand::FullHouse)
		{
			// Compare the 3-cards of both hands
			if (_First[0]->GetValue() != _Second[0]->GetValue())
				return _First[0]->GetValueInt() > _Second[0]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;

			// Compare the 2-cards of both hands
			if (_First[3]->GetValue() != _Second[3]->GetValue())
				return _First[3]->GetValueInt() > _Second[3]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;

			return ComparisonResult::Draw;
		}

		else if (FirstType == Hand::FourKind)
		{
			// Compare the 4-cards of both hands
			if (_First[0]->GetValue() != _Second[0]->GetValue())
				return _First[0]->GetValueInt() > _Second[0]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;

			// Compare the kicker card of both hands
			if (_First[4]->GetValue() != _Second[4]->GetValue())
				return _First[4]->GetValueInt() > _Second[4]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;
		
			return ComparisonResult::Draw;
		}

		else if (FirstType == Hand::StraightFlush)
		{
			//Compare the last card AKA largest value in both hands
			if (_First[4]->GetValue() != _Second[4]->GetValue())
				return _First[4]->GetValueInt() > _Second[4]->GetValueInt() ? ComparisonResult::Win : ComparisonResult::Lose;

			return ComparisonResult::Draw;
		}

		else if (FirstType == Hand::RoyalFlush)
		{
			return ComparisonResult::Draw;
		}
	}

	std::array<Card*,5> HandsEvaluator::GetBestCommunalHand(std::array<Card*,2> _Hole, std::array<Card*,5> _Communal)
	{
		//5 community cards
		std::vector<std::array<Card*,5>> PossibleHands;
		std::array<Card*,5> BestHand = SortHand(_Communal);

		//Combination with first or second hole card
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[2], _Communal[3] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[2], _Communal[4] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0], _Communal[0], _Communal[1], _Communal[3], _Communal[4] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0], _Communal[0], _Communal[2], _Communal[3], _Communal[4] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0], _Communal[1], _Communal[2], _Communal[3], _Communal[4] }));
											
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[2], _Communal[3] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[2], _Communal[4] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[1], _Communal[0], _Communal[1], _Communal[3], _Communal[4] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[1], _Communal[0], _Communal[2], _Communal[3], _Communal[4] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[1], _Communal[1], _Communal[2], _Communal[3], _Communal[4] }));
											 
		//Combination with both hole cards	 
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0],_Hole[1],_Communal[0],_Communal[1],_Communal[2] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0],_Hole[1],_Communal[0],_Communal[1],_Communal[3] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0],_Hole[1],_Communal[0],_Communal[1],_Communal[4] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0],_Hole[1],_Communal[0],_Communal[2],_Communal[3] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0],_Hole[1],_Communal[0],_Communal[2],_Communal[4] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0],_Hole[1],_Communal[0],_Communal[3],_Communal[4] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0],_Hole[1],_Communal[1],_Communal[2],_Communal[3] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0],_Hole[1],_Communal[1],_Communal[2],_Communal[4] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0],_Hole[1],_Communal[1],_Communal[3],_Communal[4] }));
		PossibleHands.push_back(SortHand(*new std::array<Card*,5>{ _Hole[0],_Hole[1],_Communal[2],_Communal[3],_Communal[4] }));

		for (unsigned int Index = 0; Index < PossibleHands.size(); Index++)
		{
			if (CompareHand(PossibleHands[Index], BestHand) == ComparisonResult::Win)
				BestHand = PossibleHands[Index];
		}

		return BestHand;
	}

	std::string GetHandTypeStr(Hand _Type)
	{
		if (_Type == Hand::High) return "High";
		else if (_Type == Hand::Pair) return "Pair";
		else if (_Type == Hand::TwoPair) return "Two Pair";
		else if (_Type == Hand::ThreeKind) return "Three of a kind";
		else if (_Type == Hand::Flush) return "Flush";
		else if (_Type == Hand::Straight) return "Straight";
		else if (_Type == Hand::FullHouse) return "Full House";
		else if (_Type == Hand::StraightFlush) return "Straight Flush";
		else if (_Type == Hand::FourKind) return "Four of a kind";
		else if (_Type == Hand::RoyalFlush) return "Royal flush";

		return "Hand not found";
	}

	std::string GetHandStr(std::array<Card*, 5> _Hand)
	{
		std::string HandStr = "";

		for (unsigned int Index = 0; Index < 5; Index++)
		{
			HandStr += _Hand[Index]->GetInfo();
			if (Index != _Hand.size() - 1) HandStr += ",";
		}

		return HandStr;
	}

	std::string GetCardsStr(std::vector<Card*> _Cards)
	{
		std::string CardStr = "";
		
		for (unsigned int Index = 0; Index < _Cards.size(); Index++)
		{
			CardStr += _Cards[Index]->GetInfo();
			if (Index != _Cards.size() - 1) CardStr += ",";
		}

		return CardStr;
	}

	std::string GetCardsIntStr(Card** _Hand)
	{
		return std::to_string(_Hand[0]->GetValueInt()) + "," + std::to_string(_Hand[1]->GetValueInt()) + "," + std::to_string(_Hand[2]->GetValueInt()) + "," + std::to_string(_Hand[3]->GetValueInt()) + std::to_string(_Hand[4]->GetValueInt());
	}

	std::string GetCardsIntStr(std::vector<Card*> _Hand)
	{
		return std::to_string(_Hand[0]->GetValueInt()) + "," + std::to_string(_Hand[1]->GetValueInt()) + "," + std::to_string(_Hand[2]->GetValueInt()) + "," + std::to_string(_Hand[3]->GetValueInt()) + std::to_string(_Hand[4]->GetValueInt());
	}
}