#include <algorithm>
#include <cinttypes>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <vector>

struct Card
{
    std::set<std::size_t> winningNumbers;
    std::set<std::size_t> ticketNumbers;
    std::size_t           cardNumber;
    std::size_t           matches    = 0;
    std::size_t           cardsCount = 1;
};

std::vector<Card> loadInput()
{
    std::fstream input("input.txt");
    std::string  line;

    std::vector<Card> cards;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            std::string       token;
            std::stringstream iss(line);
            iss >> token;  // Push out "Card" string
            iss >> token;  // Push out "x:"
            Card card;
            bool parsingWinningNumbers = true;
            while (!iss.eof())
            {
                iss >> token;
                if (token == "|")
                {
                    parsingWinningNumbers = false;
                }
                else
                {
                    card.cardNumber = cards.size();
                    auto number    = std::stoul(token);
                    if (parsingWinningNumbers)
                    {
                        card.winningNumbers.insert(number);
                    }
                    else
                    {
                        card.ticketNumbers.insert(number);
                    }
                }
            }
            cards.push_back(std::move(card));
        }
    }
    return cards;
}

size_t partOne(std::vector<Card>& cards)
{
    std::size_t score = 0;

    for (auto& card : cards)
    {
        std::vector<std::size_t> intersection;
        std::set_intersection(
            card.winningNumbers.begin(),
            card.winningNumbers.end(),
            card.ticketNumbers.begin(),
            card.ticketNumbers.end(),
            std::back_inserter(intersection));

        if (!intersection.empty())
        {
            score += 1 << (intersection.size() - 1);
        }

        card.matches = intersection.size();
    }

    return score;
}

size_t partTwo(std::vector<Card>& cards)
{
    for (auto& card : cards)
    {
        if (card.matches != 0)
        {
            // Increment the score for each card won by the times the current card was won until now.
            for (size_t i = (card.cardNumber + 1); i <= ( card.cardNumber+ card.matches); i++)
            {
                cards[i].cardsCount += card.cardsCount;
            }
        }
    }

    std::size_t score = std::accumulate(cards.begin(), cards.end(), 0, [](std::size_t score, Card const& card){
        return score + card.cardsCount;
    });
    return score;
}

int main()
{
    auto cards = loadInput();
    std::cout << "First Part " << partOne(cards) << std::endl;
    std::cout << "Second Part " << partTwo(cards) << std::endl;
    return 0;
}
