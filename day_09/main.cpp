#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

using Report  = std::vector<int64_t>;
using Reports = std::vector<Report>;

Reports parseInput()
{
    Reports      reports;
    std::fstream input("input.txt");
    std::string  line;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            // split
            std::stringstream ss{line};
            Report            report;
            std::string       token;
            while (!ss.eof())
            {
                ss >> token;
                report.push_back(std::stoll(token));
            }
            reports.push_back(std::move(report));
        }
    }
    return reports;
}

// The idea for predictor is to recursively store diffs into a new vector until all of them are 0,
// and on the way back append the value to the current vector which is the sum of the last value of the vector of
// level lower and and the last value of this level

int64_t predictNextValue(Report report)
{
    if (std::all_of(report.begin(), report.end(), [](int64_t value) { return value == 0; }))
    {
        // Reached the end
        return 0;
    }

    Report difference;
    difference.reserve(report.size() - 1);
    for (auto itr = report.begin() + 1; itr != report.end(); itr++)
    {
        difference.push_back(*itr - *(itr - 1));
    }
    return report.back() + predictNextValue(difference);
}

int64_t predictPreviousValue(Report report)
{
    if (std::all_of(report.begin(), report.end(), [](int64_t value) { return value == 0; }))
    {
        // Reached the end
        return 0;
    }

    Report difference;
    difference.reserve(report.size() - 1);
    for (auto itr = report.begin() + 1; itr != report.end(); itr++)
    {
        difference.push_back(*itr - *(itr - 1));
    }
    return report.front() - predictPreviousValue(difference);
}

using Predictor = std::function<int64_t(Report)>;

int64_t sumOfPredictedValues(Reports const& reports, Predictor predictor)
{
    return std::accumulate(reports.begin(), reports.end(), 0, [&predictor](int64_t sum, Report const& report) {
        return sum + predictor(report);
    });
}

int main()
{
    auto input = parseInput();

    std::cout << "First part:  " << sumOfPredictedValues(input, predictNextValue) << std::endl;
    std::cout << "Second part:  " << sumOfPredictedValues(input, predictPreviousValue) << std::endl;
    return 0;
}