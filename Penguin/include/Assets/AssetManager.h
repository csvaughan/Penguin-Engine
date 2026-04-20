#pragma once
#include "pgpch.h"
#include "Log/Log.h"
#include "Audio/AudioSystem.h"
#include "Audio/AudioType.h"
#include "Memory/Ref.h"

struct SDL_Renderer;

namespace pgn 
{   
    class Font;
    class Texture;
    class Sound;
    class Animation;

    class AssetManager 
    {
    public:
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;
    
        // --- Texture API ---
        void loadTexture(const std::string& id, const std::string& filename);
        Ref<Texture> getTexture(const std::string& id); 
        void removeTexture(const std::string& id);

        // --- Font API ---
        void loadFont(const std::string& id, const std::string& filename, float ptSize);
        Ref<Font> getFont(const std::string& id);    
        void removeFont(const std::string& id);

        // --- Sound API ---
        void loadSound(const std::string& id, const std::string& path);
        void loadMusic(const std::string& id, const std::string& path);
        Ref<Audio> getAudio(const std::string& id);
        void removeAudio(const std::string& id);

        // --- Animation API ---
        //void loadAnimation(const std::string& id, const std::string& path);
        //Ref<Animation> getAnimation(const std::string& id);
        //void removeAnimation(const std::string& id);
        
        void clearTextures() { m_textures.clear(); }
        void clearFonts()    { m_fonts.clear(); }
        void clearAudio()    { m_audio.clear(); }
        void clearAllAssets();

        size_t GetTextureCount()    { return m_textures.size(); };
        size_t GetFontCount()       { return m_fonts.size(); }
        size_t GetAudioCount()      { return m_audio.size(); }

    private:

        void Init(SDL_Renderer* renderer, const std::string& resourcPath);
        void Shutdown();
        std::filesystem::path resolvePath(const std::string& filename);
        void loadAudio(const std::string& id, const std::string& filename, AudioType type);
        void loadDefaults(); 

        AssetManager() = default;
        ~AssetManager() = default; 

        SDL_Renderer* m_renderer = nullptr;
        std::filesystem::path m_resourcePath;

        //Asset Storage
        std::unordered_map<std::string, WeakRef<Texture>>   m_textures;
        std::unordered_map<std::string, WeakRef<Font>>      m_fonts;
        std::unordered_map<std::string, WeakRef<Audio>>     m_audio;
        //std::unordered_map<std::string, WeakRef<Animation>> m_animations;

        std::unordered_map<std::string, Ref<Texture>> m_textureCache;
        std::unordered_map<std::string, Ref<Font>> m_fontCache;
        std::unordered_map<std::string, Ref<Audio>> m_audioCache;

        friend class Application;
    };

    // Exceptions
    class AssetNotFoundException : public std::runtime_error {
    public:
        explicit AssetNotFoundException(const std::string& id)
            : std::runtime_error("Asset Not Found: " + id) {}
    };

    class AssetLoadException : public std::runtime_error {
    public:
        explicit AssetLoadException(const std::string& id, const std::string& details)
            : std::runtime_error("Asset Load Failed [" + id + "]: " + details) {}
    };
}