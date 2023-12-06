#include <algorithm>
#include <cinttypes>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

std::vector<std::string> loadInput()
{
    std::fstream             input("input.txt");
    std::string              line;
    std::vector<std::string> inputStrings;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            inputStrings.push_back(std::move(line));
        }
    }
    return inputStrings;
}

std::size_t firstPart(std::vector<std::string> const& input)
{
    const char* digits = "0123456789";

    std::size_t row   = 0;
    std::size_t score = 0;

    for (auto const& line : input)
    {
        // find number in string
        std::size_t firstDigitPos = 0;
        std::size_t lastDigitPos  = 0;
        while ((firstDigitPos = line.find_first_of(digits, lastDigitPos)) != std::string::npos)
        {
            lastDigitPos = line.find_first_not_of(digits, firstDigitPos);  // First POS not a digit
            if (lastDigitPos == std::string::npos)
            {
                lastDigitPos = line.size();
            }

            // expand search mask by 1 field in any direction and check if symbol is contained
            auto maskBeginPos = firstDigitPos;
            auto maskEndPos   = lastDigitPos;

            std::size_t neighboringSymbols = 0;

            if (maskBeginPos != 0)
            {
                // check left
                maskBeginPos--;
                neighboringSymbols += line[maskBeginPos] != '.';
            }
            if (maskEndPos != line.size())
            {
                // check right
                neighboringSymbols += line[maskEndPos] != '.';
                maskEndPos++;  // Prepare for iterators
            }

            // check upper row
            if (row > 0)
            {
                neighboringSymbols += std::any_of(
                    input[row - 1].begin() + maskBeginPos,
                    input[row - 1].begin() + maskEndPos,
                    [](char const ch) { return ch != '.'; });
            }

            // check lower row
            if (row < (input.size() - 1))
            {
                neighboringSymbols += std::any_of(
                    input[row + 1].begin() + maskBeginPos,
                    input[row + 1].begin() + maskEndPos,
                    [](char const ch) { return ch != '.'; });
            }

            if (neighboringSymbols != 0)
            {
                // convert to number and add to sum
                std::string number(line.begin() + firstDigitPos, line.begin() + lastDigitPos);
                score += std::stol(number);
            }
        }
        row++;
    }
    return score;
}

struct PartNumber
{
    std::size_t value;
    std::size_t columnBegin;
    std::size_t columnEnd;

    friend bool operator<(PartNumber const& left, PartNumber const& right)
    {
        return left.columnBegin < right.columnBegin;
    }
};

struct PartNumbers
{
    std::vector<PartNumber> getAdjacentTo(std::size_t column)
    {
        std::vector<PartNumber> foundNumbers;
        auto iter = std::lower_bound(mPartNumbers.begin(), mPartNumbers.end(), PartNumber{0, column, 0});
        // Check if any numbers are found past column
        if (iter != mPartNumbers.end())
        {
            // Check if found number is adjacent to colum
            if ((iter->columnBegin == column) || (iter->columnBegin == (column + 1)))
            {
                foundNumbers.push_back(*iter);
            }
        }

        // If this is not a first number in row, check if previous number is adjacent to column form left side
        if (iter != mPartNumbers.begin())
        {
            // check left
            std::advance(iter, -1);
            if ((iter->columnEnd >= column - 1))
            {
                foundNumbers.push_back(*iter);
            }
        }
        return foundNumbers;
    }

    std::vector<PartNumber> mPartNumbers;
};

std::size_t secondPart(std::vector<std::string> const input)
{
    std::map<std::size_t, PartNumbers>               numbersInRow;
    std::vector<std::pair<std::size_t, std::size_t>> gears;

    std::size_t row = 0;
    for (auto const& line : input)
    {
        std::optional<std::size_t> number;
        std::optional<std::size_t> columnBegin;
        for (std::size_t column = 0; column < line.size(); column++)
        {
            if (std::isdigit(line[column]))
            {
                if (!number)
                {
                    // start of the number
                    number      = line[column] - '0';
                    columnBegin = column;
                }
                else
                {
                    *number *= 10;
                    *number += line[column] - '0';
                }
            }
            else if (number)  // finished parsing a number
            {
                // Store the parsed PartNumber
                numbersInRow[row].mPartNumbers.push_back(PartNumber{*number, *columnBegin, column - 1});
                // Reset optionals
                number      = {};
                columnBegin = {};
            }

            if (line[column] == '*')
            {
                // This is a gear symbol
                gears.emplace_back(row, column);
            }
        }

        // Check if number reaches the end of the row
        if (number)
        {
            numbersInRow[row].mPartNumbers.push_back(PartNumber{*number, *columnBegin, line.size() - 1});
        }
        row++;
    }

    // Go over all gears and search for adjacent numbers in row, row+1 and row - 1;
    std::size_t score = 0;
    for (auto& gear : gears)
    {
        auto partNumbers = numbersInRow[gear.first].getAdjacentTo(gear.second);
        if (gear.first > 0)
        {
            auto previousRow = numbersInRow[gear.first - 1].getAdjacentTo(gear.second);
            partNumbers.insert(partNumbers.end(), previousRow.begin(), previousRow.end());
        }
        if (gear.first < (input.size() - 1))
        {
            auto nextRow = numbersInRow[gear.first + 1].getAdjacentTo(gear.second);
            partNumbers.insert(partNumbers.end(), nextRow.begin(), nextRow.end());
        }

        if (partNumbers.size() == 2)
        {
            score += partNumbers.front().value * partNumbers.back().value;
        }
    }
    return score;
}

int main()
{
    auto input = loadInput();
    std::cout << "First part: " << firstPart(input) << std::endl;
    std::cout << "Second part: " << secondPart(input) << std::endl;
    return 0;
}