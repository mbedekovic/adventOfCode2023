#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <set>
#include "input.hpp"

size_t transform(std::size_t x, Mappings const& mappings)
{
    for (auto const& mapping : mappings)
    {
        std::size_t srcBegin = mapping.src;
        std::size_t srcEnd   = srcBegin + mapping.len;

        // If input is in source range, map to destination range
        if ((x >= srcBegin) && (x < srcEnd))
        {
            return mapping.dst + (x - srcBegin);
        }
    }
    // If no mappings matched, return the same number
    return x;
}

size_t partOne()
{
    std::set<std::size_t> locations;

    for (auto seed : seeds)
    {
        auto soil        = transform(seed, seedToSoil);
        auto fertilizer  = transform(soil, soilToFertilizer);
        auto water       = transform(fertilizer, fertilizerToWater);
        auto light       = transform(water, waterToLight);
        auto temperature = transform(light, lightToTemperature);
        auto humidity    = transform(temperature, temperatureToHumidity);
        auto location    = transform(humidity, humidityToLocation);
        locations.insert(location);
    }
    return *locations.begin();
}

struct Range
{
    Range(std::size_t begin, std::size_t end)
    : begin(begin)
    , end(end)
    {
    }

    std::size_t begin;
    std::size_t end;

    std::size_t size() const
    {
        return end - begin;
    }

    bool empty() const
    {
        return begin == end;
    }

    friend bool operator<(Range const& left, Range const& right)
    {
        return left.begin < right.begin;
    }
};

// Check if left range intersects with right range
std::optional<Range> intersection(Range const& left, Range const& right)
{
    if (left.begin <= right.begin && right.begin < left.end)
    {
        // left range is either super set of right or they have a part intersection
        if (left.end <= right.end)
        {
            return Range{right.begin, left.end};
        }
        return right;
    }

    if (right.begin <= left.begin && left.begin < right.end)
    {
        // right range is either superset of left they have a par intersection
        if (right.end <= left.end)
        {
            return Range{left.begin, right.end};
        }
        return left;
    }

    return {};
}

using Ranges         = std::vector<Range>;
using OptionalRanges = std::vector<std::optional<Range>>;
using OrderedRanges  = std::set<Range>;

OrderedRanges transformRange(Range const& srcRange, Mapping const& mapping)
{
    OrderedRanges mappedRange;
    auto          section = intersection(srcRange, Range{mapping.src, mapping.src + mapping.len});
    if (section)
    {
        // input range intersects with mapping source range
        // do the mapping on intersection
        std::size_t offset = section->begin - mapping.src;  // intersection always start either at src or past it.
        mappedRange.insert(Range(mapping.dst + offset, mapping.dst + offset + section->size()));

        //? Also include unmapped parts of the range.
        //! I still think this is necessary to get the general solution, but I got the correct solution for my input without it.
        //! Including non-mapped parts of range causes additional problems when the same initial range is affected by multiple mappings.
        // Range before{srcRange.begin, section->begin};
        // if (!before.empty())
        // {
        //     mappedRange.insert(before);
        // }

        // Range after{section->end, srcRange.end};
        // if (!after.empty())
        // {
        //     mappedRange.insert(after);
        // }
    }
    return mappedRange;
}

OrderedRanges transformRangesForMapping(OrderedRanges const& ranges, Mappings const& mappings)
{
    OrderedRanges transformedRanges;
    for (auto const& range : ranges)
    {
        OrderedRanges outputRanges;
        for (auto const& mapping : mappings)
        {
            auto mappedRanges = transformRange(range, mapping);
            outputRanges.insert(mappedRanges.begin(), mappedRanges.end());
        }
        if (outputRanges.empty())
        {
            // no mapping took place, output range is equal to input range
            outputRanges.insert(range);
        }

        transformedRanges.insert(outputRanges.begin(), outputRanges.end());
    }
    return transformedRanges;
}

size_t partTwo()
{
    std::set<std::size_t> locations;

    OrderedRanges seedRanges;

    for (auto seedItr = seeds.begin(); seedItr != seeds.end();)
    {
    size_t seedBegin = *seedItr;
    seedItr++;
    size_t seedEnd = seedBegin + *seedItr;
    seedItr++;

    seedRanges.emplace(seedBegin, seedEnd);
    }

    // seedRanges.emplace(seeds[0], seeds[0] + seeds[1]);

    auto soilRanges        = transformRangesForMapping(seedRanges, seedToSoil);
    auto fertilizerRanges  = transformRangesForMapping(soilRanges, soilToFertilizer);
    auto waterRanges       = transformRangesForMapping(fertilizerRanges, fertilizerToWater);
    auto lightRanges       = transformRangesForMapping(waterRanges, waterToLight);
    auto temperatureRanges = transformRangesForMapping(lightRanges, lightToTemperature);
    auto humidityRanges    = transformRangesForMapping(temperatureRanges, temperatureToHumidity);
    auto locationRanges    = transformRangesForMapping(humidityRanges, humidityToLocation);

    // std::set is always ordered in ascending order.
    return locationRanges.begin()->begin;

}

int main()
{
    std::cout << "First part " << partOne() << std::endl;
    std::cout << "Second part " << partTwo() << std::endl;
    return 0;
}