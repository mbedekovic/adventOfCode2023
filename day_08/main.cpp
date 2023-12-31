/**
 * This is basically a graph. The trick is to build a graph and follow the path of predetermined
 * steps until ZZZ is reached.
 * Second part is not realistic to simulate/brute force. The trick was to recognize that each path is cyclic
 * and detect the cycle for each of the paths for nodes starting with (**A) and ending with (**Z)
 */
#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <vector>

struct Node
{
    std::string value;
    std::string left;
    std::string right;
};

bool operator<(Node const& left, Node const& right)
{
    return left.value < right.value;
}

bool operator<(Node const left, std::string const& key)
{
    return left.value < key;
}

bool operator<(std::string const& key, Node const& right)
{
    return key < right.value;
}

using Network = std::set<Node, std::less<>>;

// Didn't really feel like parsing this part, so C/P
std::string getInstructions()
{
    return "LLRLRRRLRRRLRRLLRRRLLRRLLRLRLRRRLRRRLLRRRLLRRRLRRLRRLRLRRLLRRRLRRRLLRRRLRRLLLRRLRLLLRLRRRLRLRLLLRRLRRLLLRRRLLRRRLRLRLLRRLRLRRRLRLRLLRLRRLRRRLRRLRLRRRLRLRRLRRLRLRRLLRLRLRRLRLLRRLRRLRLRRLLRLRLLRRLLRLLLRRLRLRRRLRRRLRRRLRLRLRRRLLLRLRRLRLRRRLRRRLRRRLRLRRRLRRRLRRRLRRRR";
}

Network parseNetwork()
{
    Network      network;
    std::fstream input("input.txt");
    std::string  line;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            // split
            std::stringstream ss{line};
            std::string       token;

            // Node
            Node node;
            ss >> node.value;
            ss >> token;  // discard =
            ss >> token;  // (left,
            node.left = token.substr(1, 3);
            ss >> token;  // right)
            node.right = token.substr(0, 3);

            network.insert(node);
        }
    }

    return network;
}

class Solver
{
public:
    Solver(Network const& network, std::string const& instructions)
    : mNetwork(network)
    , mInstructions(instructions)
    {
    }

    size_t findPath()
    {
        return findNode(mInstructions.begin(), "AAA");
    }

    std::vector<std::pair<Node, size_t>> findPaths()
    {
        std::vector<std::pair<Node, size_t>> cycles;
        for (auto const& node : mNetwork)
        {
            if (node.value.back() == 'A')
            {
                cycles.emplace_back(findCycle(mInstructions.begin(), node));
            }
        }
        return cycles;
    }

private:
    size_t findNode(std::string::const_iterator command, std::string const& node)
    {
        auto nodeItr = mNetwork.find(node);
        if (nodeItr == mNetwork.end())
        {
            throw std::out_of_range("Got lost in the network");
        }

        if (nodeItr->value == "ZZZ")
        {
            return 0;
        }

        if (command == mInstructions.end())
        {
            // wrap around if end is reached
            command = mInstructions.begin();
        }

        if (*command == 'L')
        {
            return 1 + findNode(++command, nodeItr->left);
        }
        else
        {
            return 1 + findNode(++command, nodeItr->right);
        }
    }

    std::pair<Node, size_t> findCycle(std::string::const_iterator command, Node const& node)
    {
        if (node.value.back() == 'Z')
        {
            // Found the first number ending with Z
            return {node, 0};
        }

        if (command >= mInstructions.end())
        {
            command = mInstructions.begin();
        }

        if (*command == 'L')
        {
            auto& leftNode         = *mNetwork.find(node.left);
            auto  lastNodeAndCycle = findCycle(++command, leftNode);
            lastNodeAndCycle.second++;
            return lastNodeAndCycle;
        }
        else
        {
            auto& rightNode        = *mNetwork.find(node.right);
            auto  lastNodeAndCycle = findCycle(++command, rightNode);
            lastNodeAndCycle.second++;
            return lastNodeAndCycle;
        }
    }

private:
    Network const&     mNetwork;
    std::string const& mInstructions;
};

int main()
{
    auto network      = parseNetwork();
    auto instructions = getInstructions();

    Solver solver(network, instructions);
    auto   stepsTakenPartOne = solver.findPath();
    auto   endNodesAndCycles = solver.findPaths();

    // Calculate least common multiple of all cycles
    auto lcm = endNodesAndCycles.front().second;
    for (auto itr = endNodesAndCycles.begin() + 1; itr != endNodesAndCycles.end(); itr++)
    {
        lcm = std::lcm(lcm, itr->second);
    }

    std::cout << "First part: " << stepsTakenPartOne << std::endl;
    std::cout << "Second part: " << lcm << std::endl;
    return 0;
}