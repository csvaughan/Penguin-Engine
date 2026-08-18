#include "Renderer/Texture.h"
#include <SDL3_image/SDL_image.h>

namespace pgn
{
    Texture::Texture(SDL_Renderer* renderer, const std::filesystem::path& filepath) 
    {
        m_handle = IMG_LoadTexture(renderer, filepath.string().c_str());
        
        if (!m_handle) {
            throw std::runtime_error("Texture Load Error (" + filepath.string() + "): " + SDL_GetError());
        }

        SDL_SetTextureBlendMode(m_handle, SDL_BLENDMODE_BLEND);
        
        updateDimensions();
    }

    Texture::Texture(SDL_Renderer* renderer, SDL_Surface* surface) 
    {
        if (!surface) throw std::runtime_error("Cannot create texture from nullptr surface");

        m_handle = SDL_CreateTextureFromSurface(renderer, surface);
        if (!m_handle) {
            throw std::runtime_error("Texture Creation Error: " + std::string(SDL_GetError()));
        }

        SDL_SetTextureBlendMode(m_handle, SDL_BLENDMODE_BLEND);
        
        updateDimensions();
    }

    void Texture::destroy() 
    {
        if (m_handle) 
        { 
            SDL_DestroyTexture(m_handle); 
            m_handle = nullptr; 
        }
    }

    void Texture::updateDimensions() 
    {
        if (m_handle) 
            SDL_GetTextureSize(m_handle, &m_width, &m_height);
    }
} // namespace pgn
