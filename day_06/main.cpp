#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <cstdlib>
#include <iostream>

struct RaceParams
{
    int64_t time;
    int64_t distance;
};

// distance = speed * time;
// distance is fixed, time we can vary. Speed is directly proportional to time when button is pressed.
/**
 * time_pressed + time_traveling = time;
 * speed = distance / time traveling;
 * distance > record_distance;
 * ======================
 * speed * time_traveling > record_distance
 * speed is directly proportional to time pressed
 * so substituting this we get:
 * (time - time_traveling) * time_traveling > record_distance;
 *
 * Multiplying and re-arranging we get a quadratic inequality
 *
 * time_traveling^2 - time_traveling * time + record_distance < 0
 *
 * Solving for zero points we get the limits of the time_traveling and winning traveling times are between them
 */

std::pair<int64_t, int64_t> quadraticZeroPoints(int64_t a, int64_t b, int64_t c)
{
    auto factor = std::sqrt(b * b - 4 * a * c);

    auto first  = ((-1 * b - factor) / (2 * a));
    auto second = ((-1 * b + factor) / (2 * a));

    int64_t first_int = std::ceil(first);
    int64_t second_int = std::floor(second);

    // This is required in case the zero points are exact integers
    if ((first_int - first) == 0)
    {
        ++first_int;
    }

    if ((second_int - second) == 0)
    {
        --second_int;
    }

    return {first_int, second_int};
}

size_t findWiningSituations(RaceParams const& input)
{
    auto winningRange = quadraticZeroPoints(1, -1 * input.time, input.distance);
    return winningRange.second - winningRange.first + 1;
}

int main()
{
    // std::vector<RaceParams> input{{7, 9}, {15, 40}, {30, 200}}; // sample input
    std::vector<RaceParams> input{{40,215}, {70,1051}, {98, 2147}, {79, 1005}};

    size_t partOne = 1;
    for (auto const& raceParam : input)
    {
        partOne *= findWiningSituations(raceParam);
    }

    // RaceParams partTwoInput{71530, 940200}; // sample input
    RaceParams partTwoInput{40709879, 215105121471005};

    std::cout << "Part one: " << partOne << std::endl;
    std::cout << "Part two: " << findWiningSituations(partTwoInput) << std::endl;


}