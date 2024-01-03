#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <vector>

using Point = std::pair<int64_t, int64_t>;

std::pair<std::vector<Point>, Point> parseInput()
{
    std::fstream       input("input.txt");
    std::string        line;
    size_t             row = 0;
    Point              sizePoint{0, 0};
    std::vector<Point> galaxys;

    while (std::getline(input, line))
    {
        size_t col = 0;
        if (!line.empty())
        {
            col = line.find('#', col);
            while (col != std::string::npos)
            {
                galaxys.emplace_back(row, col);
                col = line.find('#', col + 1);
            }
        }
        sizePoint = {row++, line.size()};
    }
    sizePoint.first = row;
    return {galaxys, sizePoint};
}

size_t sumDistancesBetweenGalaxies(
    std::vector<Point> const& galaxyPoints,
    Point const&              mapSize,
    int64_t                   expansionFactor = 1)
{
    // Determine which rows an columns are empty
    std::vector<int64_t> emptyRows;
    for (int64_t row = 0; row < mapSize.first; row++)
    {
        if (std::all_of(galaxyPoints.begin(), galaxyPoints.end(), [row](Point const& point) {
                return point.first != row;
            }))
        {
            emptyRows.push_back(row);
        }
    }

    std::vector<int64_t> emptyColumns;
    for (int64_t col = 0; col < mapSize.second; col++)
    {
        if (std::all_of(galaxyPoints.begin(), galaxyPoints.end(), [col](Point const& point) {
                return point.second != col;
            }))
        {
            emptyColumns.push_back(col);
        }
    }

    if (expansionFactor > 1)
        expansionFactor--;

    size_t distanceSum = 0;

    for (auto firstItr = galaxyPoints.begin(); firstItr != (galaxyPoints.end() - 1); firstItr++)
    {
        for (auto secondItr = firstItr + 1; secondItr < galaxyPoints.end(); secondItr++)
        {
            int64_t rowFirst = std::count_if(emptyRows.begin(), emptyRows.end(), [&](int64_t const row) {
                return row < firstItr->first;
            });

            int64_t rowSecond = std::count_if(emptyRows.begin(), emptyRows.end(), [&](int64_t const row) {
                return row < secondItr->first;
            });

            int64_t colFirst = std::count_if(emptyColumns.begin(), emptyColumns.end(), [&](int64_t const col) {
                return col < firstItr->second;
            });

            int64_t colSecond = std::count_if(emptyColumns.begin(), emptyColumns.end(), [&](int64_t const col) {
                return col < secondItr->second;
            });

            auto distance = std::abs(
                                (secondItr->first + expansionFactor * rowSecond)
                                - (firstItr->first + expansionFactor * rowFirst))
                            + std::abs(
                                (secondItr->second + expansionFactor * colSecond)
                                - (firstItr->second + expansionFactor * colFirst));

            distanceSum += distance;
        }
    }
    return distanceSum;
}

int main()
{
    auto input = parseInput();

    std::cout << "First part: " << sumDistancesBetweenGalaxies(input.first, input.second) << std::endl;
    std::cout << "Second part: " << sumDistancesBetweenGalaxies(input.first, input.second, 1000000) << std::endl;

    return 0;
}
