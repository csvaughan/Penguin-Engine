#pragma once
#include "Math/Vector.h"
#include "Asset/IAsset.h"

namespace pgn 
{
    class Texture : public IAsset
    {
    public:
        Texture(SDL_Renderer* renderer, const std::filesystem::path& filepath);
        Texture(SDL_Renderer* renderer, SDL_Surface* surface);

        ~Texture() { destroy(); }

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        SDL_Texture* getSDLTexture() const { return m_handle; }
        float getWidth() const { return m_width; }
        float getHeight() const { return m_height; }
        Vector2 getSize() const { return {m_width, m_height}; }

    private:
        void destroy();
        void updateDimensions();

        SDL_Texture* m_handle = nullptr;
        float m_width = 0;
        float m_height = 0;
    };
}