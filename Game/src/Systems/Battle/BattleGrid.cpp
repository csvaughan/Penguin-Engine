/*#include "Game/Battle/BattleGrid.h"

BattleGrid::BattleGrid(int widthInTiles, int heightInTiles, sf::Vector2f tileSizePixels, sf::Vector2f worldPosition)
    : m_width(widthInTiles), m_height(heightInTiles), m_tileSize(tileSizePixels), m_position(worldPosition)
{
    //initialize grid
    m_tiles.reserve(m_width * m_height);
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            sf::Vector2i gridPos(x, y);
            sf::Vector2f worldPos = {m_position.x + static_cast<float>(x * m_tileSize.x), m_position.y + static_cast<float>(y * m_tileSize.y)};
            m_tiles.emplace_back(gridPos, worldPos, m_tileSize);
            GridTile& tile = m_tiles.back();
            tile.type = TileType::Ground;
            tile.occupyingCombatant = nullptr;
            tile.highlighted = false;
        }
    }
}

// gridToWorld: Converts grid (x,y) to world pixel (x,y) accounting for grid's m_position
sf::Vector2f BattleGrid::gridToWorld(const sf::Vector2i& gridPos) const
{
    return sf::Vector2f(m_position.x + static_cast<float>(gridPos.x * m_tileSize.x) + (m_tileSize.x / 2.0f),
                        m_position.y + static_cast<float>(gridPos.y * m_tileSize.y) + (m_tileSize.y / 2.0f));
}

// worldToGrid: Converts world pixel (x,y) to grid (x,y) accounting for grid's m_position
// Returns the grid coordinates of the tile containing the worldPos
sf::Vector2i BattleGrid::worldToGrid(const sf::Vector2f& worldPos) const
{
    return sf::Vector2i(static_cast<int>((worldPos.x - m_position.x) / m_tileSize.x), 
                        static_cast<int>((worldPos.y - m_position.y) / m_tileSize.y));
}

bool BattleGrid::isValidGridPos(const sf::Vector2i& gridPos) const
{
    return gridPos.x >= 0 && gridPos.x < m_width && gridPos.y >= 0 && gridPos.y < m_height;
}

GridTile& BattleGrid::getTile(const sf::Vector2i& gridPos)
{
    if (!isValidGridPos(gridPos)) {
        static GridTile invalidTile; // Return a static invalid tile for error cases
        std::cerr << "Error: Attempted to access invalid grid position (" << gridPos.x << ", " << gridPos.y << ")" << std::endl;
        return invalidTile;
    }
    return m_tiles[calculate1DIndex(gridPos.x, gridPos.y)];
}

const GridTile& BattleGrid::getTile(const sf::Vector2i& gridPos) const
{
    if (!isValidGridPos(gridPos)) {
        static const GridTile invalidTile;
        std::cerr << "Error: Attempted to access invalid grid position (" << gridPos.x << ", " << gridPos.y << ") (const)" << std::endl;
        return invalidTile;
    }
    return m_tiles[calculate1DIndex(gridPos.x, gridPos.y)];
}

bool BattleGrid::isTileWalkable(const sf::Vector2i& gridPos) const
{
    return (isValidGridPos(gridPos)) ? getTile(gridPos).type == TileType::Ground && !getTile(gridPos).occupyingCombatant : false; // Only ground and unoccupied are walkable
}

void BattleGrid::freeTile(const sf::Vector2i& gridPos) {
    if (isValidGridPos(gridPos)) {
        GridTile& tile = getTile(gridPos);
        tile.occupyingCombatant = nullptr;
    }
}

void BattleGrid::occupyTile(const sf::Vector2i& gridPos, Engine::ECS::Entity* combatant) {
    if (isValidGridPos(gridPos)) {
        GridTile& tile = getTile(gridPos);
        tile.occupyingCombatant = combatant;
    }
}

const sf::Vector2i BattleGrid::getCombatantTilePosition(Engine::ECS::Entity* c) const
{
    for (const auto& tile : m_tiles) 
    {
        if(c == tile.occupyingCombatant){return tile.gridPosition;}
    }
    return {-1,-1}; // If actor not found occupying any tile, return an invalid position
}

void BattleGrid::highlightTile(const sf::Vector2i& gridPos, const sf::Color& color)
{
    if (isValidGridPos(gridPos))
    {
        GridTile& tile = getTile(gridPos);
        tile.highlighted = true;
        tile.highlightColor = color;
    }
}

void BattleGrid::unhighlightTile(const sf::Vector2i& gridPos)
{
    if (isValidGridPos(gridPos))
    {
        GridTile& tile = getTile(gridPos);
        tile.highlighted = false;
        tile.highlightColor = sf::Color(0, 100, 255, 120);
    }
}

void BattleGrid::clearAllHighlights()
{
    for (auto& tile : m_tiles){tile.highlighted = false;}
}

void BattleGrid::render() 
{
    for (auto& tile : m_tiles) 
    {
        // Determine fill color based on tile state
        sf::Color fillColor = (tile.highlighted) ? tile.highlightColor : sf::Color(50, 50, 50, 80); 
        if(tile.occupyingCombatant) {fillColor = sf::Color::Red;}
        tile.shape.setFillColor(fillColor);
        Engine::Renderer::submit(tile.shape);
    }
}*/