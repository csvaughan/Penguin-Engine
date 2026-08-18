#pragma once
#include "IAsset.h"
#include "Renderer/Font.h"
#include "Renderer/Texture.h"
#include "Audio/Audio.h"
#include "Audio/AudioSystem.h"


namespace pgn
{
    class IAssetImporter {
    public:
        virtual ~IAssetImporter() = default;
        virtual Ref<IAsset> load(const std::filesystem::path& path) = 0;
    };

    class TextureImporter : public IAssetImporter {
    public:
        explicit TextureImporter(SDL_Renderer* renderer) : m_renderer(renderer) {}

        Ref<IAsset> load(const std::filesystem::path& path) override 
        {
            return CreateRef<Texture>(m_renderer, path); 
        }
    private:
        SDL_Renderer* m_renderer = nullptr;
    };

    class FontImporter : public IAssetImporter {
    public:
        explicit FontImporter(SDL_Renderer* renderer, float ptsize) : m_renderer(renderer), m_ptsize(ptsize) {}

        Ref<IAsset> load(const std::filesystem::path& path) override 
        {
            return CreateRef<Font>(m_renderer, path, m_ptsize); 
        }
    private:
        SDL_Renderer* m_renderer = nullptr;
        float m_ptsize;
    };

    class SoundEffectImporter : public IAssetImporter {
    public:
        Ref<IAsset> load(const std::filesystem::path& path) override {
            return CreateRef<SoundEffect>(
                AudioSystem::GetEngine(), 
                AudioSystem::GetSFXGroup(), 
                path.string()
            );
        }
    };

    class MusicImporter : public IAssetImporter {
    public:
        Ref<IAsset> load(const std::filesystem::path& path) override {
            return CreateRef<Music>(
                AudioSystem::GetEngine(), 
                AudioSystem::GetMusicGroup(), 
                path.string()
            );
        }
    };
} // namespace name
