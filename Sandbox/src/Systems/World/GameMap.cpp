#include "GameMap.h"
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

bool GameMap::load(const std::string& path, const TextureMap& pool) 
{
    tmx::Map map;
    if (!map.load(path)) return false;

    m_width = map.getTileCount().x;
    m_height = map.getTileCount().y;
    m_tileSize = { (float)map.getTileSize().x, (float)map.getTileSize().y };
    
    m_chunks.clear();
    m_tilePrototypes.clear();
    m_tileGrid.assign(m_width * m_height, TileFlags::None);

    // 1. Setup Logic Prototypes
    for (const auto& ts : map.getTilesets()) {
        for (const auto& t : ts.getTiles()) {
            TileInfo info;
            info.gid = ts.getFirstGID() + t.ID;
            for (const auto& p : t.properties) {
                if (p.getName() == "Walkable" && p.getBoolValue()) info.flags = info.flags | TileFlags::Walkable;
                if (p.getName() == "Water" && p.getBoolValue())    info.flags = info.flags | TileFlags::Water;
            }
            m_tilePrototypes[info.gid] = info;
        }
    }

    // 2. Initialize Chunk Grid
    int chunksX = (m_width + CHUNK_SIZE - 1) / CHUNK_SIZE;
    int chunksY = (m_height + CHUNK_SIZE - 1) / CHUNK_SIZE;
    m_chunks.resize(chunksX * chunksY);

    for (int y = 0; y < chunksY; ++y) {
        for (int x = 0; x < chunksX; ++x) {
            auto& chunk = m_chunks[y * chunksX + x];
            chunk.bounds = { 
                (float)x * CHUNK_SIZE * m_tileSize.x, (float)y * CHUNK_SIZE * m_tileSize.y,
                (float)CHUNK_SIZE * m_tileSize.x, (float)CHUNK_SIZE * m_tileSize.y 
            };
        }
    }

    // 3. Process Layers
    int zIndex = 0;
    for (const auto& layer : map.getLayers()) 
    {
        if (layer->getType() == tmx::Layer::Type::Tile) 
        {
            const auto& tileLayer = layer->getLayerAs<tmx::TileLayer>();
            
            // Logic Pass
            const auto& tiles = tileLayer.getTiles();
            for (size_t i = 0; i < tiles.size(); ++i) {
                if (tiles[i].ID != 0 && m_tilePrototypes.count(tiles[i].ID))
                    m_tileGrid[i] = m_tileGrid[i] | m_tilePrototypes[tiles[i].ID].flags;
            }

            // Rendering Pass (Spatially distributed)
            distributeTilesToChunks(map, tileLayer, pool, zIndex++, chunksX);
        }
    }
    return true;
}

void GameMap::distributeTilesToChunks(const tmx::Map& map, const tmx::TileLayer& layer, const TextureMap& pool, int zIndex, int chunksX) 
{
    const auto& tiles = layer.getTiles();

    int chunksY = (m_height + CHUNK_SIZE - 1) / CHUNK_SIZE;

    // Temporary structure to group vertices by [ChunkID][TexturePath]
    std::unordered_map<int, std::unordered_map<std::string, pgn::VertexArray>> chunkWork;
    chunkWork.reserve(chunksX * chunksY);

    for (size_t i = 0; i < tiles.size(); ++i) 
    {
        uint32_t gid = tiles[i].ID;
        if (gid == 0) continue;

        const tmx::Tileset* ts = getTilesetForGID(map, gid);
        if (!ts) continue;

        int tileX = i % m_width;
        int tileY = i / m_width;
        
        // Horizontal index + (Vertical index * row width)
        int chunkIdx = (tileX / CHUNK_SIZE) + (tileY / CHUNK_SIZE) * chunksX;

        // UV Math
        uint32_t lid = gid - ts->getFirstGID();
        float tsW = (float)ts->getTileSize().x;
        float tsH = (float)ts->getTileSize().y;
        
        // Margin and Spacing support
        float px = (float)ts->getMargin() + (lid % ts->getColumnCount()) * (tsW + ts->getSpacing());
        float py = (float)ts->getMargin() + (lid / ts->getColumnCount()) * (tsH + ts->getSpacing());

        float u0 = px / ts->getImageSize().x;
        float v0 = py / ts->getImageSize().y;
        float u1 = (px + tsW) / ts->getImageSize().x;
        float v1 = (py + tsH) / ts->getImageSize().y;

        // World Position
        float x = (float)tileX * m_tileSize.x;
        float y = (float)tileY * m_tileSize.y;

        // Flipping Logic
        pgn::Vector2 uv0 = {u0, v0}, uv1 = {u1, v0}, uv2 = {u1, v1}, uv3 = {u0, v1};
        auto flip = tiles[i].flipFlags;
        if (flip & tmx::TileLayer::FlipFlag::Diagonal)   std::swap(uv1, uv3);
        if (flip & tmx::TileLayer::FlipFlag::Horizontal) { std::swap(uv0, uv1); std::swap(uv3, uv2); }
        if (flip & tmx::TileLayer::FlipFlag::Vertical)   { std::swap(uv0, uv3); std::swap(uv1, uv2); }

        // Use the VertexArray from the specific chunk and texture
        chunkWork[chunkIdx][ts->getImagePath()].addQuad(
            { {x, y},                               pgn::Color::White, uv0 },
            { {x + m_tileSize.x, y},                pgn::Color::White, uv1 },
            { {x + m_tileSize.x, y + m_tileSize.y}, pgn::Color::White, uv2 },
            { {x, y + m_tileSize.y},                pgn::Color::White, uv3 }
        );
    }

    // Move working arrays into the actual chunks
    for (auto& [cIdx, textureMap] : chunkWork) 
    {
        for (auto& [path, va] : textureMap) 
        {
            if (pool.contains(path)) 
                m_chunks[cIdx].batches.push_back({ std::move(va), pool.at(path), zIndex });
        }
    }
}

const std::vector<MapRenderView>& GameMap::GetVisibleViews(const pgn::FloatRect& cameraBounds) 
{
    m_viewCache.clear(); 
    
    for (const auto& chunk : m_chunks) 
    {
        if (chunk.bounds.intersects(cameraBounds)) 
        {
            for (const auto& batch : chunk.batches) 
                m_viewCache.push_back({ batch.va, batch.texture, batch.zIndex });
        }
    }
    return m_viewCache;
}

const tmx::Tileset* GameMap::getTilesetForGID(const tmx::Map& map, uint32_t gid) const 
{
    for (const auto& ts : map.getTilesets()) 
        if (gid >= ts.getFirstGID() && gid < ts.getFirstGID() + ts.getTileCount()) return &ts;
    return nullptr;
}

TileFlags GameMap::GetFlagsAt(pgn::Vector2 worldPos) const 
{
    pgn::Vector2 tp = WorldToTile(worldPos);
    if (tp.x < 0 || tp.x >= m_width || tp.y < 0 || tp.y >= m_height) return TileFlags::None;
    return m_tileGrid[static_cast<size_t>(tp.y * m_width + tp.x)];
}

pgn::Vector2 GameMap::WorldToTile(pgn::Vector2 wp) const { return { std::floor(wp.x / m_tileSize.x), std::floor(wp.y / m_tileSize.y) }; }
pgn::Vector2 GameMap::TileToWorld(pgn::Vector2 tp) const { return { tp.x * m_tileSize.x, tp.y * m_tileSize.y }; }