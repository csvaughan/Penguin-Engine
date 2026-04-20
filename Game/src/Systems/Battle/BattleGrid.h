#pragma once
#include "EnginePCH.h"
#include "Game/Battle/Combatant.h"

enum class TileType {
    DefaultTile,
    Ground,
    Obstacle,
};

struct GridTile
{
    sf::Vector2i gridPosition;
    sf::Vector2f worldPosition; // Top-left corner of the tile in world coordinates
    sf::RectangleShape shape;
    TileType type = TileType::DefaultTile;
    Engine::ECS::Entity* occupyingCombatant = nullptr;
    bool highlighted = false;  
    sf::Color highlightColor = sf::Color(0, 100, 255, 120);

    // Constructor to initialize members, including the shape
    GridTile(sf::Vector2i pos = {-1,-1}, sf::Vector2f worldPos = {0.0f, 0.0f}, sf::Vector2f tileSize = {0.0f, 0.0f})
        : gridPosition(pos),
          worldPosition(worldPos),
          shape(tileSize) // Initialize the shape with its size
    {
        // Set initial properties for the shape
        shape.setPosition(worldPos);
        shape.setOutlineThickness(1);
        shape.setOutlineColor(sf::Color(100, 100, 100)); // Standard border
        shape.setFillColor(sf::Color(50, 50, 50, 80)); // Default semi-transparent dark gray
    }
};


class BattleGrid
{
public:
    // Constructor now takes sf::Vector2f for tile size and world position for origin
    BattleGrid(int widthInTiles, int heightInTiles, sf::Vector2f tileSizePixels, sf::Vector2f worldPosition);

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    sf::Vector2f getPosition() const {return m_position;}
    sf::Vector2f getTileSize() const { return m_tileSize;}
    sf::Vector2f getCenter() const {return m_position + sf::Vector2f{(static_cast<float>(m_width) * m_tileSize.x / 2.0f), (static_cast<float>(m_height) * m_tileSize.y / 2.0f)};} 

    sf::Vector2i worldToGrid(const sf::Vector2f& worldPos) const;
    sf::Vector2f gridToWorld(const sf::Vector2i& gridPos) const; // Returns center of tile
    bool isValidGridPos(const sf::Vector2i& gridPos) const;

    GridTile& getTile(const sf::Vector2i& gridPos);
    const GridTile& getTile(const sf::Vector2i& gridPos) const;

    bool isTileWalkable(const sf::Vector2i& gridPos) const;

    void freeTile(const sf::Vector2i& gridPos);
    void occupyTile(const sf::Vector2i& gridPos, Engine::ECS::Entity*);

    const sf::Vector2i getCombatantTilePosition(Engine::ECS::Entity*) const; // Find actor's current tile

    void highlightTile(const sf::Vector2i& gridPos, const sf::Color& color = sf::Color(0, 100, 255, 120));
    void unhighlightTile(const sf::Vector2i& gridPos);
    void clearAllHighlights();

    void render();

private:

    inline size_t calculate1DIndex(int x, int y) const { return static_cast<size_t>(y * m_width + x); }

private:
    std::vector<GridTile> m_tiles;
    unsigned int m_width, m_height;
    sf::Vector2f m_tileSize, m_position; // Top-left world position of the grid
};