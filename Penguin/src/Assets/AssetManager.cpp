#include "Assets/AssetManager.h"
#include "Assets/Texture.h"
#include "Assets/Font.h"
#include "Assets/Audio.h"
#include "Math/MathUtils.h"
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace pgn {

    // Generic Search
    template <typename T>
    inline bool searchLibrary(
        std::unordered_map<std::string, WeakRef<T>>& map, 
        std::unordered_map<std::string, Ref<T>>& cache, 
        const std::string& cacheKey,
        const std::string& id)
    {
        // 1. ID check
        if (map.find(id) != map.end()) return true;

        // 2. Cache check
        auto it = cache.find(cacheKey);
        if (it != cache.end()) 
        {
            map[id] = it->second;
            PGN_CORE_INFO("Resource linked from cache: {}", id);
            return true;
        }
        return false;
    }

    std::filesystem::path AssetManager::resolvePath(const std::string &filename)
    {
        return m_resourcePath / filename;
    }

    void pgn::AssetManager::loadDefaults()
    {
        loadTexture("debug_texture", ENGINE_RESOURCES_PATH + std::string("textures/error_texture.png"));
        loadFont("default_font", ENGINE_RESOURCES_PATH + std::string("fonts/PixelatedEleganceRegular-ovyAA.ttf"), 20);
    }

    void AssetManager::clearAllAssets()
    {
        clearTextures();
        clearFonts();
        clearAudio();
        loadDefaults();
        PGN_CORE_INFO("AssetManager: All assets cleared.");
    }

    void AssetManager::Init(SDL_Renderer *renderer, const std::string &resourcPath)
    {
        m_renderer = renderer; 
        m_resourcePath = resourcPath;

        if (!TTF_WasInit()) 
        {
            if (!TTF_Init()) 
                { PGN_CORE_ERROR("Failed to initialize SDL_ttf: {}", SDL_GetError()); return; }
        }
           
        PGN_ASSERT(!m_resourcePath.empty(),"AssetManager resource path must be set before loading assets.");

        if (!std::filesystem::exists(m_resourcePath)) 
            PGN_CORE_ERROR("Resource Path does not exist: {}", m_resourcePath.string());

        loadDefaults();
        
        PGN_CORE_INFO("Resource Path set to: {}", m_resourcePath.string());
        PGN_CORE_INFO("Asset Manager Initilized.");    
    }

    void AssetManager::Shutdown()
    {   
        // Clear the strong references (the cache)
        m_textureCache.clear();
        m_fontCache.clear();
        m_audioCache.clear();
        
        // Clear the weak references just to be clean
        m_textures.clear();
        m_fonts.clear();
        m_audio.clear();

        if (TTF_WasInit())
        {
            TTF_Quit();
        }
        
        m_renderer = nullptr;
        PGN_CORE_INFO("AssetManager Shutdown Successfully...");
    }

    // --- Texture Implementation ---
    void AssetManager::loadTexture(const std::string& id, const std::string& filename) 
    {
        if (!m_renderer) return;

        std::filesystem::path path = resolvePath(filename);
        std::string cacheKey = path.string(); // Texture key is just the path

        // Check Lib/Cache
        if (searchLibrary(m_textures, m_textureCache, cacheKey, id)) return;

        // Load
        try {
            if (!std::filesystem::exists(path)) throw std::runtime_error("File not found");
            
            auto res = CreateRef<Texture>(m_renderer, path);
            m_textures[id] = res;
            m_textureCache[cacheKey] = res;
            PGN_CORE_INFO("Texture Loaded: {}", cacheKey);
        }
        catch (const std::exception& e) {
            PGN_CORE_ERROR("ASSET ERROR: Failed to load texture '{}': {}. Using default.", id, e.what());
            m_textures[id] = m_textures["default_error"];
        }
    }

    Ref<Texture> AssetManager::getTexture(const std::string& id) 
    {
        auto it = m_textures.find(id);
        if (it == m_textures.end()) 
        {
            PGN_CORE_ERROR("ASSET ERROR: Texture ID {} not found. Using default.", id);
            return m_textures["default_error"].lock();
        }
        return it->second.lock();
    }

    void AssetManager::removeTexture(const std::string &id)
    {
        if (m_textures.erase(id) == 0) 
            PGN_CORE_WARN("Asset Warning: Attempted to remove non-existent texture ID: {}", id);
    } 
    
    
    // --- Font Implementation ---
    void AssetManager::loadFont(const std::string& id, const std::string& filename, float ptSize) 
    {
        if (!m_renderer) return;

        std::filesystem::path path = resolvePath(filename);

        std::string cacheKey = std::format("{}_{:.1f}", path.string(), ptSize);

        if (searchLibrary(m_fonts, m_fontCache, cacheKey, id)) return;

        try {
            if (!std::filesystem::exists(path)) throw std::runtime_error("File not found");

            auto res = CreateRef<Font>(m_renderer, path.string(), ptSize);
            
            if (!res->isValid()) throw std::runtime_error("TTF Generation Failed");

            m_fonts[id] = res;
            m_fontCache[cacheKey] = res;
            PGN_CORE_INFO("Font '{}' Loaded: {}", id, cacheKey);
        }
        catch (const std::exception& e) {
            PGN_CORE_ERROR("ASSET ERROR: Failed to load font '{}': {}", id, e.what());
        }
    }

    Ref<Font> AssetManager::getFont(const std::string& id) 
    {
        auto it = m_fonts.find(id);
        if (it == m_fonts.end()) 
        {
            PGN_CORE_ERROR("ASSET ERROR: Font not found: {}", id);
            return nullptr; 
        }
        return it->second.lock();
    }

    void AssetManager::removeFont(const std::string &id)
    {
        if (m_fonts.erase(id) == 0) 
            PGN_CORE_WARN("Asset Warning: Attempted to remove non-existent texture ID: {}", id);
    }

    /// --- Audio Implementation ---
    void AssetManager::loadSound(const std::string& id, const std::string& filename) 
    {
        loadAudio(id, filename, AudioType::Effect);
    }

    void AssetManager::loadMusic(const std::string& id, const std::string& filename) 
    {
        loadAudio(id, filename, AudioType::Music);
    }

    void AssetManager::loadAudio(const std::string &id, const std::string &filename, AudioType type)
    {
        std::filesystem::path path = resolvePath(filename);
        std::string cacheKey = path.string();

        if (searchLibrary(m_audio, m_audioCache, cacheKey, id)) return;

        try {
             if (!std::filesystem::exists(path)) throw std::runtime_error("File not found");

            ma_sound_group* group = (type == AudioType::Music) ? AudioSystem::GetMusicGroup() : AudioSystem::GetSFXGroup();
            auto res = CreateRef<Audio>(AudioSystem::GetEngine(), group, path, type);
            
            m_audio[id] = res;
            m_audioCache[cacheKey] = res;
            PGN_CORE_INFO("Audio Loaded: {}", cacheKey);
        }
        catch (const std::exception& e) {
            PGN_CORE_ERROR("ASSET ERROR: Failed to load audio '{}': {}", id, e.what());
        }
    }

    Ref<Audio> AssetManager::getAudio(const std::string &id) 
    {
        auto it = m_audio.find(id);
        if (it == m_audio.end())
        {
            PGN_CORE_ERROR("ASSET ERROR: Audio ID {} not found!", id);
            return nullptr;
        }
        return it->second.lock();
    }

    void AssetManager::removeAudio(const std::string &id)
    {
        if (m_audio.erase(id) == 0) 
            PGN_CORE_WARN("Asset Warning: Attempted to remove non-existent texture ID: {}", id);
    }
}