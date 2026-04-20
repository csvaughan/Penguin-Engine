#pragma once
#include "Math/Vector.h"
#include "Memory/Ref.h"
#include <SDL3_image/SDL_image.h>
#include <filesystem>

namespace pgn 
{
    class Texture : public RefCounted
    {
    public:
        Texture(SDL_Renderer* renderer, const std::filesystem::path& filepath) 
        {
            m_handle = IMG_LoadTexture(renderer, filepath.string().c_str());
            
            if (!m_handle) {
                throw std::runtime_error("Texture Load Error (" + filepath.string() + "): " + SDL_GetError());
            }
            
            updateDimensions();
        }

        Texture(SDL_Renderer* renderer, SDL_Surface* surface) 
        {
            if (!surface) throw std::runtime_error("Cannot create texture from nullptr surface");

            m_handle = SDL_CreateTextureFromSurface(renderer, surface);
            if (!m_handle) {
                throw std::runtime_error("Texture Creation Error: " + std::string(SDL_GetError()));
            }
            
            updateDimensions();
        }

        ~Texture() 
        {
            destroy();
        }

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        SDL_Texture* getSDLTexture() const { return m_handle; }
        float getWidth() const { return m_width; }
        float getHeight() const { return m_height; }
        Vector2 getSize() const { return {m_width, m_height}; }

    private:
        void destroy() 
        {
            if (m_handle) 
            { 
                SDL_DestroyTexture(m_handle); 
                m_handle = nullptr; 
            }
        }

        void updateDimensions() 
        {
            if (m_handle) 
                SDL_GetTextureSize(m_handle, &m_width, &m_height);
        }

        SDL_Texture* m_handle = nullptr;
        float m_width = 0;
        float m_height = 0;
    };
}