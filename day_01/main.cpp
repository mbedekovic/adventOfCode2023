#include <algorithm>
#include <cinttypes>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

int firstPart()
{
    std::fstream     input("input.txt");
    std::string      line;
    std::vector<int> calibrationValues;
    char const*      digits = "0123456789";
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            auto first = line.find_first_of(digits);
            auto last  = line.find_last_of(digits);

            if ((first != std::string::npos) && (last != std::string::npos))
            {
                calibrationValues.push_back(10 * (line[first] - '0') + (line[last] - '0'));
            }
        }
    }

    return std::accumulate(calibrationValues.begin(), calibrationValues.end(), 0);
}

int secondPart()
{
    std::fstream     input("input.txt");
    std::string      line;
    std::vector<int> calibrationValues;
    char const*      digits = "0123456789";
    std::array<std::string, 9> const
        digitStrings{"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            std::map<size_t, int> digitPositions;

            // Numerical digits, potentially not present
            auto first = line.find_first_of(digits);
            auto last  = line.find_last_of(digits);

            if ((first != std::string::npos) && (last != std::string::npos))
            {
                digitPositions[first] = line[first] - '0';
                digitPositions[last]  = line[last] - '0';
            }

            int idx = 0;
            std::for_each(
                digitStrings.begin(),
                digitStrings.end(),
                [&line, &idx, &digitPositions](std::string const& digit) {
                    idx++;

                    // String digit might be repeated several times, we have to iterate over a complete string for each
                    // digit substring, not just find the first one
                    for (size_t pos = 0; pos != std::string::npos;)
                    {
                        pos = line.find(digit, pos);
                        if (pos != std::string::npos)
                        {
                            // Found substring, store position
                            digitPositions[pos] = idx;
                            pos += digit.size();  // move position past the digit substring position in string
                        }
                    }
                });
            calibrationValues.push_back(digitPositions.begin()->second * 10 + digitPositions.rbegin()->second);
        }
    }

    return std::accumulate(calibrationValues.begin(), calibrationValues.end(), 0);
}

int main()
{
    std::cout << "First part: " << firstPart() << std::endl;
    std::cout << "Second part: " << secondPart() << std::endl;
}