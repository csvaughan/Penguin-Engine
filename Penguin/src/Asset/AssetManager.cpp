#include "Asset/AssetManager.h"
#include "Asset/AssetImporter.h"
#include "Renderer/Texture.h" 
#include "Renderer/Font.h"    
#include "Audio/Audio.h"  
#include "Log/Log.h"  

namespace pgn 
{

    void AssetManager::logNoImporterError(const char* typeName) 
    {
        PGN_CORE_ERROR("Asset Error: No importer registered for type {}", typeName);
    }

    void AssetManager::logFileNotFoundError(const std::string& id, const std::string& path) 
    {
        PGN_CORE_ERROR("Asset Load Failed [{}]: File not found at {}", id, path);
    }

    void AssetManager::logNullAssetError(const std::string& id, const std::string& path) 
    {
        PGN_CORE_ERROR("Asset Load Failed [{}]: Importer returned null for {}", id, path);
    }

    void AssetManager::logAssetLoadedInfo(const std::string& id, const std::string& path) 
    {
        PGN_CORE_INFO("Asset Loaded [{}]: {}", id, path);
    }

    void AssetManager::logAssetExceptionError(const std::string& id, const char* what) 
    {
        PGN_CORE_ERROR("Asset Load Exception [{}]: {}", id, what);
    }

    void AssetManager::logAssetNotFoundError(const std::string& id) 
    {
        PGN_CORE_ERROR("Asset Not Found: {}", id);
    }

    std::filesystem::path AssetManager::resolvePath(const std::string& filename) 
    {
        return m_resourcePath / filename;
    }

    void AssetManager::loadDefaults() 
    {
        // Default assets are loaded using standard generic interface
        load<Texture>("debug_texture", "textures/error_texture.png");
        load<Font>("default_font", "fonts/PixelatedEleganceRegular-ovyAA.ttf");
    }

    void AssetManager::Init(SDL_Renderer* renderer, const std::string& resourcePath) 
    {
        m_renderer = renderer;
        m_resourcePath = resourcePath;

        PGN_ASSERT(!m_resourcePath.empty(), "AssetManager resource path must be set before loading assets.");

        if (!std::filesystem::exists(m_resourcePath)) 
            PGN_CORE_ERROR("Resource Path does not exist: {}", m_resourcePath.string());

        // Register default importers with required subsystem pointers
        registerImporter<Texture, TextureImporter>(m_renderer);
        registerImporter<Font, FontImporter>(m_renderer, 25.0f);
        registerImporter<Music, MusicImporter>(); // Music
        registerImporter<SoundEffect, SoundEffectImporter>(); // Sound Effects

        loadDefaults();

        PGN_CORE_INFO("Resource Path set to: {}", m_resourcePath.string());
        PGN_CORE_INFO("AssetManager Initialized with Importer Registry.");
    }

    void AssetManager::Shutdown() 
    {
        m_assets.clear();
        m_importers.clear();
        m_renderer = nullptr;
        PGN_CORE_INFO("AssetManager Shutdown Successfully.");
    }

    bool AssetManager::has(const std::string& id) const 
    {
        return m_assets.find(id) != m_assets.end();
    }

    bool AssetManager::remove(const std::string& id) 
    {
        auto it = m_assets.find(id);
        if (it == m_assets.end()) 
        {
            PGN_CORE_WARN("Asset Warning: Attempted to remove non-existent ID: {}", id); 
            return false;
        }
        m_assets.erase(it);
        PGN_CORE_INFO("Asset Removed: {}", id);
        return true;
    }

    void AssetManager::clearAll() 
    {
        m_assets.clear();
        loadDefaults();
        PGN_CORE_INFO("AssetManager: All assets cleared.");
    }
}