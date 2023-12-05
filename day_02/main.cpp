
#include <algorithm>
#include <cinttypes>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

using Set = std::map<std::string, int>;

struct Game
{
    int              gameId;
    std::vector<Set> sets;
};

std::vector<Game> parseGame()
{
    std::vector<Game> games;
    std::fstream      input("input.txt");
    std::string       line;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            Game game;
            auto beginPos = line.find_first_of(':');
            // parse game ID
            std::string        gameIdStr(line.begin(), line.begin() + beginPos);
            std::istringstream gameIdSs(gameIdStr);
            std::string        id;
            gameIdSs >> id;  // Skip Game string
            gameIdSs >> id;
            game.gameId = std::stoi(id);

            beginPos++;
            while (beginPos != std::string::npos)
            {
                // find a set within a game and tokenize
                auto        endPos = line.find(';', beginPos + 1);
                std::string setString;
                if (endPos == std::string::npos)
                {
                    // Last set doesn't contain ';'
                    setString = std::string(line.begin() + beginPos, line.end());
                    beginPos  = std::string::npos;
                }
                else
                {
                    setString = std::string(line.begin() + beginPos, line.begin() + endPos);
                    beginPos  = endPos + 1;
                }

                std::istringstream         iss(setString);
                std::string                token;
                std::string                pair;
                std::map<std::string, int> cubes;

                while (std::getline(iss, pair, ','))
                {
                    std::istringstream tokenIss(pair);
                    tokenIss >> token;
                    int cnt = std::stoi(token);
                    tokenIss >> token;
                    cubes[token] = cnt;
                }
                game.sets.push_back(std::move(cubes));
            }
            games.push_back(std::move(game));
        }
    }
    return games;
}

int firstPart(std::vector<Game> const& games)
{
    std::map<std::string, int> const cubesCount{{"red", 12}, {"green", 13}, {"blue", 14}};

    int score = 0;
    for (auto const& game : games)
    {
        bool gamePossible = true;
        for (auto const& set : game.sets)
        {
            for (auto cubeItr : set)
            {
                if (cubeItr.second > cubesCount.at(cubeItr.first))
                {
                    gamePossible = false;
                    break;
                }
            }
        }

        if (gamePossible)
        {
            score += game.gameId;
        }
    }
    return score;
}

int secondPart(std::vector<Game> const& games)
{
    int score = 0;
    for (auto const& game : games)
    {
        Set minimumCubes{{"red", 0}, {"green", 0}, {"blue", 0}};
        for (auto const& set : game.sets)
        {
            for (auto setItr : set)
            {
                minimumCubes[setItr.first] = std::max(minimumCubes[setItr.first], setItr.second);
            }
        }
        int power = minimumCubes["red"] * minimumCubes["green"] * minimumCubes["blue"];
        score += power;
    }
    return score;
}

int main()
{
    auto const game = parseGame();
    std::cout << "First part " << firstPart(game) << std::endl;
    std::cout << "Second part " << secondPart(game) << std::endl;

    return 0;
}