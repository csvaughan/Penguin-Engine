/*#include "Game/Utils/Pathfinder.h"
#include <algorithm> 

inline bool isValidPosition(const BattleGrid& grid, const sf::Vector2i& pos) 
{
    return true;//grid.isValidGridPos(pos) && grid.isTileWalkable(pos);
}

GridPath Pathfinder::findPath(const BattleGrid& grid, const sf::Vector2i& startGridPos, const sf::Vector2i& targetGridPos, int maxDistance) 
{
    // checks for valid start/target positions are the same
    if (!isValidPosition(grid, targetGridPos)) {return {};}
    if (startGridPos == targetGridPos) {return {startGridPos};}

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
    std::unordered_map<sf::Vector2i, float> gCosts;
    std::unordered_map<sf::Vector2i, sf::Vector2i> cameFrom;

    gCosts[startGridPos] = 0.0f;
    openList.push(Node(startGridPos, 0.0f, calculateHCost(startGridPos, targetGridPos), startGridPos));

    while (!openList.empty())
    {
        Node currentNode = openList.top();
        openList.pop();

        if (currentNode.position == targetGridPos) 
        {
            //path reconstruction logic 
            GridPath path;
            sf::Vector2i temp = targetGridPos;
            while (temp != startGridPos) 
            {
                path.push_back(temp);
                temp = cameFrom[temp];
            }
            path.push_back(startGridPos);
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        sf::Vector2i neighbors[4] = {
            
            {currentNode.position.x, currentNode.position.y - 1}, // Up
            {currentNode.position.x, currentNode.position.y + 1}, // Down
            {currentNode.position.x - 1, currentNode.position.y}, // Left
            {currentNode.position.x + 1, currentNode.position.y}  // Right
        };

        for (const auto& neighborPos : neighbors) 
        {
            if (isValidPosition(grid, neighborPos))
            { 
                float tentativeGCost = currentNode.gCost + 1.0f;
                // If maxDistance is set and this path is too long, skip it.
                if (maxDistance >= 0 && tentativeGCost > maxDistance)
                {
                    continue; // Stop exploring this path
                }

                if (gCosts.find(neighborPos) == gCosts.end() || tentativeGCost < gCosts[neighborPos]) 
                {
                    gCosts[neighborPos] = tentativeGCost;
                    cameFrom[neighborPos] = currentNode.position;
                    openList.push(Node(neighborPos, tentativeGCost, calculateHCost(neighborPos, targetGridPos), currentNode.position));
                }
            }
        }
    }

    return {}; // No path found (or no path found within the distance cap)
}*/