/**
 * It's required to find a point o a closed path which is the furthest away from the start point.
 * Problematic point is that a map contains "pipes" which may lead to different paths. So the first order of
 * business is determining the direction which is on the closed path.
 * On the plus side there there is only movement along the cartesian coordinates (manhattan).
 * Also another fact is that to one point on the cycle there is exactly one pipe leading in and one pipe leading out
 *
 * Steps to solve:
 * Map out the cyclic path from staring point.
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <vector>

using Map   = std::vector<std::string>;
using Point = std::pair<size_t, size_t>;

std::pair<Map, Point> parseInput()
{
    std::fstream input("input.txt");
    std::string  line;
    Map          fieldMap;
    Point        startPoint{0, 0};
    size_t       row = 0;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            auto column = line.find('S');
            if (column != std::string::npos)
            {
                // Found starting point
                startPoint.first  = row;
                startPoint.second = column;
            }
            fieldMap.push_back(std::move(line));
            row++;
        }
    }

    return {fieldMap, startPoint};
}

using Direction = std::pair<int, int>;
Point operator+(Point const& point, Direction const& direction)
{
    Point newPoint = point;
    newPoint.first += direction.first;
    newPoint.second += direction.second;

    return newPoint;
}

class Solver
{
public:
    Solver(Map const& fieldMap)
    : mMap(fieldMap)
    {
    }

public:
    size_t findLoopLength(Point startingPoint)
    {
        // find direction from starting point

        // check starting direction towards north
        if (startingPoint.first > 0)
        {
            Direction  startDirection = {-1, 0};
            Point      nextPoint      = startingPoint + startDirection;
            char const pipe           = mMap.at(nextPoint.first).at(nextPoint.second);
            if (pipe == '|' || pipe == '7' || pipe == 'F')
            {
                return findLoop(startingPoint, startDirection);
            }
        }
        // Check south
        if (startingPoint.first < (mMap.size() - 1))
        {
            Direction  startDirection = {1, 0};
            Point      nextPoint      = startingPoint + startDirection;
            char const pipe           = mMap.at(nextPoint.first).at(nextPoint.second);
            if (pipe == '|' || pipe == 'J' || pipe == 'L')
            {
                return findLoop(startingPoint, startDirection);
            }
        }

        // check west
        if (startingPoint.second > 0)
        {
            Direction  startDirection = {0, -1};
            Point      nextPoint      = startingPoint + startDirection;
            char const pipe           = mMap.at(nextPoint.first).at(nextPoint.second);
            if (pipe == '-' || pipe == 'F' || pipe == 'L')
            {
                return findLoop(startingPoint, startDirection);
            }
        }

        // check east
        if (startingPoint.second < (mMap.front().size() - 1))
        {
            Direction  startDirection = {0, 1};
            Point      nextPoint      = startingPoint + startDirection;
            char const pipe           = mMap.at(nextPoint.first).at(nextPoint.second);
            if (pipe == '-' || pipe == 'J' || pipe == '7')
            {
                return findLoop(startingPoint, startDirection);
            }
        }
        return 0;  // Starting point leads nowhere
    }

    // The second part is a problem of finding if a point is contained within a polygon.
    // To test this one can pick a point on the map, project a ray starting from this point towards
    // an edge of a map and count the number of times the ray crosses the edge of the polygon.
    size_t findAreaWithinLoop()
    {
        Point  point, begin, end;
        size_t area = 0;
        std::sort(mLoop.begin(), mLoop.end());
        std::tie(begin, end) = getLoopRectangle();
        for (auto row = begin.first; row < end.first; row++)
        {
            for (auto col = begin.second; col < end.second; col++)
            {
                point = {row, col};
                if (!pointOnPolygon(point) && pointInPolygon(point))
                {
                    area++;
                }
            }
        }
        return area;
    }

private:
    bool pointOnPolygon(Point const& point)
    {
        return std::binary_search(mLoop.begin(), mLoop.end(), point);
    }

    // Algorithm for Jordan curve theorem taken from: https://wrfranklin.org/Research/Short_Notes/pnpoly.html
    bool pointInPolygon(Point const& point)
    {
        int  i, j, nvert = mVertices.size();
        bool c = false;

        for (i = 0, j = nvert - 1; i < nvert; j = i++)
        {
            if (((mVertices[i].first > point.first) != (mVertices[j].first > point.first))
                && (point.second < (mVertices[j].second - mVertices[i].second) * (point.first - mVertices[i].first)
                                           / (mVertices[j].first - mVertices[i].first)
                                       + mVertices[i].second))
                c = !c;
        }

        return c;
    }

    // Get the area of the map where the loop is contained, no need to search outside of the loop
    std::pair<Point, Point> getLoopRectangle()
    {
        Point maximum{0, 0};
        Point minimum{mMap.size(), mMap.front().size()};

        for (auto const& loopPoint : mLoop)
        {
            if (loopPoint.first < minimum.first)
            {
                minimum.first = loopPoint.first;
            }
            if (loopPoint.second < minimum.second)
            {
                minimum.second = loopPoint.second;
            }
            if (loopPoint.first > maximum.first)
            {
                maximum.first = loopPoint.first;
            }
            if (loopPoint.second > maximum.second)
            {
                maximum.second = loopPoint.second;
            }
        }

        return {minimum, maximum};
    }

private:
    size_t findLoop(Point currentPoint, Direction direction)
    {
        // stop condition is reaching the starting point again
        size_t loopLength = 0;
        do
        {
            auto       nextPoint = currentPoint + direction;
            char const pipe      = mMap.at(nextPoint.first).at(nextPoint.second);
            enum class CountDirection
            {
                Clockwise        = 1,
                CounterClockwise = -1
            };
            Direction newDirection;
            // check for row up movement
            if (direction.first < 0)
            {
                // When going north we can continue north, turn west or east. (|, 7, F)
                if (pipe == '|')
                    newDirection = {-1, 0};  // continue going up
                if (pipe == '7')
                    newDirection = {0, -1};  // turn left
                if (pipe == 'F')
                    newDirection = {0, 1};  // turn right
            }

            if (direction.first > 0)
            {
                // Going south: continue south, turn west, turn east (|, J, L)
                if (pipe == '|')
                    newDirection = {1, 0};
                if (pipe == 'L')
                    newDirection = {0, 1};
                if (pipe == 'J')
                    newDirection = {0, -1};
            }

            if (direction.second < 0)
            {
                // Going west, continue west, turn south, turn north (-, F, L)
                if (pipe == '-')
                    newDirection = {0, -1};
                if (pipe == 'F')
                    newDirection = {1, 0};
                if (pipe == 'L')
                    newDirection = {-1, 0};
            }

            if (direction.second > 0)
            {
                // Going est: continue east, turn south, turn north (-, 7, J)
                if (pipe == '-')
                    newDirection = {0, 1};
                if (pipe == '7')
                    newDirection = {1, 0};
                if (pipe == 'J')
                    newDirection = {-1, 0};
            }

            mLoop.push_back(currentPoint);
            if ((pipe != '.') && (pipe != '|') && (pipe != '-'))
            {
                mVertices.push_back(nextPoint);
            }
            loopLength++;
            currentPoint = nextPoint;
            direction    = newDirection;
        } while (mMap.at(currentPoint.first).at(currentPoint.second) != 'S');

        return loopLength;
    }

private:
    Map const&         mMap;
    std::vector<Point> mLoop;
    std::vector<Point> mVertices;
};

int main()
{
    auto input = parseInput();

    Solver solver(input.first);
    auto   loopLength = solver.findLoopLength(input.second);
    std::cout << "First part: " << loopLength / 2 << std::endl;
    std::cout << "Second part: " << solver.findAreaWithinLoop() << std::endl;
    return 0;
}