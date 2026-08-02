/*#include "Game/Battle/BattlePathfinder.h"
#include "Game/Battle/BattleGrid.h" 
#include <algorithm> 

bool isValidTarget(const BattleGrid& grid, const sf::Vector2i& pos, Team pathfinderTeam) 
{
        if (!grid.isValidGridPos(pos)) return false;
        
        // If the tile is completely empty/walkable, it's a valid target.
        if (grid.isTileWalkable(pos) && grid.getTile(pos).occupyingCombatant == nullptr) return true;

        // If not empty, check if it's occupied by an active enemy.
        auto& combatant = grid.getTile(pos).occupyingCombatant->GetComponent<CombatantComponent>();
        if (combatant.state != CombantantState::Defeated && combatant.team != pathfinderTeam) {
            return true; // Occupied by an active enemy, valid target for attack
        }
        
        return false; // Otherwise, invalid target (e.g., wall, ally, defeated enemy, non-walkable terrain)
}

inline float calculateHCost(const sf::Vector2i& a, const sf::Vector2i& b) {
    return static_cast<float>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
}

inline bool isTraversable(const BattleGrid& grid, const sf::Vector2i& pos) 
{
        // A tile is traversable if it's within bounds and no combatant (ally or enemy) occupies it.
        return grid.isValidGridPos(pos) && grid.isTileWalkable(pos) && grid.getTile(pos).occupyingCombatant == nullptr;
}

GridPath BattlePathfinder::findPath(const BattleGrid& grid, const sf::Vector2i& startGridPos, const sf::Vector2i& targetGridPos, int maxDistance, Team pathfinderTeam)
{
    // Initial checks for start/target positions
    if (!isValidTarget(grid, targetGridPos, pathfinderTeam)) {
        return {}; // Target is not a valid final destination (e.g., wall, ally, out of bounds)
    }
    
    // The start position must always be traversable by the combatant itself.
    // Assuming a combatant's own tile is always valid to start a path from.
    if (!grid.isValidGridPos(startGridPos)) {
        return {};
    }
    // If the start tile is occupied by an *enemy*, it's an invalid start for movement.
    // If it's occupied by an *ally*, and your game doesn't allow "phasing through" allies
    // (which `isTraversable` prevents for intermediate steps), then it's also not ideal.
    // For simplicity, we just ensure it's a valid grid position.
    // The `isTraversable` check is more for intermediate path nodes.

    if (startGridPos == targetGridPos) {
        // If the target is the start position itself, and it's a valid target (e.g., attacking self, or just standing)
        return {startGridPos};
    }

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
    std::unordered_map<sf::Vector2i, float> gCosts;
    std::unordered_map<sf::Vector2i, sf::Vector2i> cameFrom;

    gCosts[startGridPos] = 0.0f;
    openList.push(Node(startGridPos, 0.0f, calculateHCost(startGridPos, targetGridPos), startGridPos));

    while (!openList.empty())
    {
        Node currentNode = openList.top();
        openList.pop();

        // If we reached the target, reconstruct path
        if (currentNode.position == targetGridPos)
        {
            GridPath path;
            sf::Vector2i temp = targetGridPos;
            while (temp != startGridPos)
            {
                path.push_front(temp); // Use push_front for deque to build path in correct order
                temp = cameFrom[temp];
            }
            path.push_front(startGridPos); // Add start position
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
            // First, ensure the neighbor is within grid bounds.
            if (!grid.isValidGridPos(neighborPos)) continue;

            // Determine if this neighbor is the *final* target tile.
            bool isNeighborTheFinalTarget = (neighborPos == targetGridPos);

            // Pathfinding Rule:
            // An intermediate tile must be "traversable" (empty and walkable).
            // The final target tile can be "isValidTarget" (empty/walkable or occupied by enemy).
            if (!isTraversable(grid, neighborPos) && !isNeighborTheFinalTarget) {
                continue; // This neighbor is blocked (by terrain, wall, or another combatant)
                          // AND it's not the final target, so we cannot path through it.
            }
            // If it IS the final target, the `if (currentNode.position == targetGridPos)` check above will handle reaching it.
            // So, if we are at `currentNode` and `neighborPos` is the `targetGridPos`, and `isValidTarget` is true for it,
            // we should add it to the openList.

            float tentativeGCost = currentNode.gCost + 1.0f;

            // Check maxDistance (movement range)
            if (maxDistance >= 0 && tentativeGCost > maxDistance)
            {
                // If the path to this neighbor exceeds maxDistance, skip it.
                // This applies whether it's an intermediate step or the final target.
                continue;
            }

            if (gCosts.find(neighborPos) == gCosts.end() || tentativeGCost < gCosts[neighborPos])
            {
                gCosts[neighborPos] = tentativeGCost;
                cameFrom[neighborPos] = currentNode.position;
                openList.push(Node(neighborPos, tentativeGCost, calculateHCost(neighborPos, targetGridPos), currentNode.position));
            }
        }
    }

    return {}; // No path found
}*/