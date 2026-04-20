#include "GameArea.h"
#include <tmxlite/Map.hpp>

void AreaSystem::LoadLevelMap(const std::string& tmxPath) 
{
    tmx::Map loader;
    if (!loader.load(tmxPath)) 
    {
        PGN_ERROR("AreaSystem: Could not load TMX file: {0}", tmxPath);
        return;
    }

    // 1. Prepare a pool of textures for the map
    std::unordered_map<std::string, pgn::Ref<pgn::Texture>> texturePool;
    
    for (const auto& ts : loader.getTilesets()) 
    {
        std::string path = ts.getImagePath();
        // Load into AssetManager if not already there
        App().LoadTexture(path, path);
        // Grab the shared pointer to inject into the map
        texturePool[path] = App().GetTexture(path);
    }

    // 2. Initialize the Map Data
    m_currentMap = std::make_unique<GameMap>();
    if (!m_currentMap->load(tmxPath, texturePool)) 
    {
        PGN_ERROR("AreaSystem: Failed to initialize GameMap logic.");
        m_currentMap.reset();
    }
}

void AreaSystem::OnUpdate(pgn::Timestep ts) 
{
    // Handle logic like NPC triggers or animated tiles here
}

void AreaSystem::OnRender(float alpha) 
{
    if (!m_currentMap) return;

    for (const auto& v :m_currentMap->GetVisibleViews(m_camera.getViewport()))
    {
        pgn::Renderer::Submit(v.va, v.texture, v.zIndex);
    }
}