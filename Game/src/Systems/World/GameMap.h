#pragma once
#include "Penguin.h"

namespace tmx {class Map; class TileLayer; class Tileset;}

enum class TileFlags : uint16_t {
    None        = 0,
    Walkable    = 1 << 0,
    Water       = 1 << 1,
    Hazard      = 1 << 2
};

inline TileFlags operator|(TileFlags a, TileFlags b) { return static_cast<TileFlags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b)); }

struct TileInfo {
    uint32_t gid = 0;
    TileFlags flags = TileFlags::None;
};

struct RenderBatch {
    pgn::VertexArray va;
    pgn::Ref<pgn::Texture> texture;
    int zIndex = 0;
};

struct MapChunk {
    pgn::FloatRect bounds;
    std::vector<RenderBatch> batches;
};

struct MapRenderView {
    std::reference_wrapper<const pgn::VertexArray> va; 
    pgn::Ref<pgn::Texture> texture;
    int zIndex;
};

using TextureMap = std::unordered_map<std::string, pgn::Ref<pgn::Texture>>;

class GameMap {
public:
    bool load(const std::string& path, const TextureMap& pool);
    const std::vector<MapRenderView>& GetVisibleViews(const pgn::FloatRect& cameraBounds);

    TileFlags GetFlagsAt(pgn::Vector2 worldPos) const;
    pgn::Vector2 WorldToTile(pgn::Vector2 worldPos) const;
    pgn::Vector2 TileToWorld(pgn::Vector2 tilePos) const;

private:
    void distributeTilesToChunks(const tmx::Map& map, const tmx::TileLayer& layer, const TextureMap& pool, int zIndex, int chunksX);
    
    const tmx::Tileset* getTilesetForGID(const tmx::Map& map, uint32_t gid) const;

    const int CHUNK_SIZE = 16; 
    std::vector<MapChunk> m_chunks;
    std::vector<TileFlags> m_tileGrid; 
    std::unordered_map<uint32_t, TileInfo> m_tilePrototypes;

    std::vector<MapRenderView> m_viewCache;
    
    pgn::Vector2 m_tileSize;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};