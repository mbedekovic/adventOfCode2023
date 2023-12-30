#include <algorithm>
#include <cinttypes>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <vector>

enum class HandType
{
    HighCard = 0,
    OnePair,
    TwoPair,
    ThreeOfAKind,
    FullHouse,
    FourOfAKind,
    FiveOfAKind
};

// Debug print helper
constexpr std::ostream& operator<<(std::ostream& stream, HandType type)
{
    switch (type)
    {
    case HandType::HighCard:
        stream << "HighCard";
        break;
    case HandType::OnePair:
        stream << "OnePair";
        break;
    case HandType::TwoPair:
        stream << "TwoPair";
        break;
    case HandType::ThreeOfAKind:
        stream << "ThreeOfAKind";
        break;
    case HandType::FullHouse:
        stream << "FullHouse";
        break;
    case HandType::FourOfAKind:
        stream << "FourOfAKind";
        break;
    case HandType::FiveOfAKind:
        stream << "FiveOfAKind";
        break;
    default:
        break;
    }
    return stream;

}

struct HandBid
{
    std::string hand;
    size_t      bid;
    HandType    type;
};

HandType calculateHandType(std::string const& hand)
{
    std::map<char, unsigned int> cardsCount;
    for (char ch : hand)
    {
        cardsCount[ch] = cardsCount[ch] + 1;
    }

    // determine card type
    auto countCards = [&cardsCount](size_t targetCount) {
        return std::count_if(cardsCount.begin(), cardsCount.end(), [targetCount](auto const& mapPair) {
            return mapPair.second == targetCount;
        });
    };

    // check for five of a kind
    if (countCards(5) > 0)
    {
        return HandType::FiveOfAKind;
    }

    if (countCards(4) > 0)
    {
        return HandType::FourOfAKind;
    }

    if (countCards(3) && countCards(2))
    {
        return HandType::FullHouse;
    }

    if (countCards(3) > 0)
    {
        return HandType::ThreeOfAKind;
    }

    if (countCards(2) > 1)
    {
        return HandType::TwoPair;
    }

    if (countCards(2) == 1)
    {
        return HandType::OnePair;
    }

    return HandType::HighCard;
}

HandType calculateHandTypeWithJoker(std::string const& hand)
{
    std::map<char, unsigned int> cardsCount;
    for (char ch : hand)
    {
        cardsCount[ch] = cardsCount[ch] + 1;
    }

    auto jokerCount = std::count(hand.begin(), hand.end(), 'J');

    // determine card type
    auto countCards = [&cardsCount](size_t targetCount) {
        return std::count_if(cardsCount.begin(), cardsCount.end(), [targetCount](auto const& mapPair) {
            return mapPair.second == targetCount;
        });
    };

    if (countCards(5) > 0)
    {
        return HandType::FiveOfAKind;
    }

    // Joker can raise the level of the hand
    if (countCards(4) > 0)
    {
        if (jokerCount == 4)
        {
            return HandType::FiveOfAKind;
        }
        if (jokerCount == 1)
        {
            return HandType::FiveOfAKind;
        }
        return HandType::FourOfAKind;
    }

    if (countCards(3) && countCards(2))
    {
        if (jokerCount == 3 || jokerCount == 2)
        {
            return HandType::FiveOfAKind;
        }
        return HandType::FullHouse;
    }

    if (countCards(3) > 0)
    {
        if (jokerCount == 3)
        {
            return HandType::FourOfAKind;
        }
        if (jokerCount == 2)
        {
            return HandType::FullHouse;
        }
        if (jokerCount == 1)
        {
            return HandType::FourOfAKind;
        }
        return HandType::ThreeOfAKind;
    }

    if (countCards(2) > 1)
    {
        if (jokerCount == 2)
        {
            // one of tha pairs are joker, turn it into four of a kind
            return HandType::FourOfAKind;
        }
        if (jokerCount == 1)
        {
            return HandType::FullHouse;
        }
        return HandType::TwoPair;
    }

    if (countCards(2) == 1)
    {
        if (jokerCount == 2)
        {
            return HandType::ThreeOfAKind;
        }
        if (jokerCount == 1)
        {
            return HandType::ThreeOfAKind;
        }
        return HandType::OnePair;
    }

    if (jokerCount == 1)
    {
        return HandType::OnePair;
    }
    return HandType::HighCard;
}

struct ComparePart1 : public std::binary_function<HandBid, HandBid, bool>
{
    bool operator()(HandBid const& left, HandBid const& right) const
    {
        if (left.type < right.type)
        {
            return true;
        }

        if (left.type > right.type)
        {
            return false;
        }

        // Hands are of equal type, calculate first higher card
        static const std::map<char, unsigned int> cardLookup{
            {'2', 0},
            {'3', 1},
            {'4', 2},
            {'5', 3},
            {'6', 4},
            {'7', 5},
            {'8', 6},
            {'9', 7},
            {'T', 8},
            {'J', 9},
            {'Q', 10},
            {'K', 11},
            {'A', 12}};

        for (std::size_t idx = 0; idx < left.hand.size(); idx++)
        {
            if (cardLookup.at(left.hand[idx]) < cardLookup.at(right.hand[idx]))
            {
                // left is weaker than right
                return true;
            }

            if (cardLookup.at(left.hand[idx]) > cardLookup.at(right.hand[idx]))
            {
                // right is higher than left
                return false;
            }
        }
        return false;  // Should never be reached otherwise cards are the same
    }
};

std::set<HandBid, ComparePart1> parseGameInputPart1()
{
    std::set<HandBid, ComparePart1> hands;
    std::fstream                    input("input.txt");
    std::string                     line;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            // split
            HandBid           inputHand;
            std::stringstream ss{line};
            ss >> inputHand.hand;
            std::string token;
            ss >> token;
            inputHand.bid = std::stoull(token);

            inputHand.type = calculateHandType(inputHand.hand);

            hands.insert(inputHand);
        }
    }
    return hands;
}

struct ComparePart2 : public std::binary_function<HandBid, HandBid, bool>
{
    bool operator()(HandBid const& left, HandBid const& right) const
    {
        if (left.type < right.type)
        {
            return true;
        }

        if (left.type > right.type)
        {
            return false;
        }

        // Hands are of equal type, calculate first higher card
        static const std::map<char, unsigned int> cardLookup{
            {'J', 0},
            {'2', 1},
            {'3', 2},
            {'4', 3},
            {'5', 4},
            {'6', 5},
            {'7', 6},
            {'8', 7},
            {'9', 8},
            {'T', 9},
            {'Q', 10},
            {'K', 11},
            {'A', 12}};

        for (std::size_t idx = 0; idx < left.hand.size(); idx++)
        {
            if (cardLookup.at(left.hand[idx]) < cardLookup.at(right.hand[idx]))
            {
                // left is weaker than right
                return true;
            }

            if (cardLookup.at(left.hand[idx]) > cardLookup.at(right.hand[idx]))
            {
                // right is higher than left
                return false;
            }
        }
        return false;  // Should never be reached otherwise cards are the same
    }
};

std::set<HandBid, ComparePart2> parseGameInputPart2()
{
    std::set<HandBid, ComparePart2> hands;
    std::fstream                    input("input.txt");
    std::string                     line;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            // split
            HandBid           inputHand;
            std::stringstream ss{line};
            ss >> inputHand.hand;
            std::string token;
            ss >> token;
            inputHand.bid = std::stoull(token);

            inputHand.type = calculateHandTypeWithJoker(inputHand.hand);

            hands.insert(inputHand);
        }
    }
    return hands;
}

int main()
{
    auto handsPart1 = parseGameInputPart1();

    size_t winningsPart1 = 0;
    size_t rank          = 1;

    winningsPart1 = std::accumulate(handsPart1.begin(), handsPart1.end(), 0, [&rank](auto const& sum, HandBid const& hand) {
        return sum + hand.bid * rank++;
    });

    std::cout << "First part: " << winningsPart1 << std::endl;

    auto handsPart2 = parseGameInputPart2();

    size_t winningsPart2 = 0;
    rank          = 1;

    winningsPart2 = std::accumulate(handsPart2.begin(), handsPart2.end(), 0, [&rank](auto const& sum, HandBid const& hand) {
        return sum + hand.bid * rank++;
    });

    std::cout << "Second part: " << winningsPart2 << std::endl;

    return 0;
}