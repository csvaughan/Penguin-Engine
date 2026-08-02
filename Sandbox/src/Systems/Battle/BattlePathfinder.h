#pragma once
#include "EnginePCH.h"
#include "Game/Battle/Combatant.h"

using GridPath = std::deque<sf::Vector2i>;

class BattleGrid;

// hash function for sf::Vector2i to use it in unordered_map
namespace std 
{
    template <>
    struct hash<sf::Vector2i> 
    {
        size_t operator()(const sf::Vector2i& v) const {
            // A simple hash combination for two integers
            return hash<int>()(v.x) ^ (hash<int>()(v.y) << 1);
        }
    };
}

// Helper struct for A* priority queue
struct Node 
{
    sf::Vector2i position;
    float gCost; // Cost from start to this node
    float hCost; // Heuristic cost from this node to end
    float fCost; // gCost + hCost
    sf::Vector2i parent; // To reconstruct the path

    // Constructor
    Node(sf::Vector2i pos, float g, float h, sf::Vector2i p): position(pos), gCost(g), hCost(h), fCost(g + h), parent(p) {}

    // For priority queue: nodes with lower fCost have higher priority
    bool operator>(const Node& other) const {return fCost > other.fCost;}
};

class BattlePathfinder 
{
public:
    // Modified signature to include the team of the pathfinding combatant
    static GridPath findPath(const BattleGrid& grid, const sf::Vector2i& startGridPos, const sf::Vector2i& targetGridPos, int maxDistance, Team pathfinderTeam);
};