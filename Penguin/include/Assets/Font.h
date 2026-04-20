#pragma once
#include <glm/vec2.hpp>
#include "Memory/Ref.h"
#include "Math/Rect.h"
#include "pgpch.h"

struct SDL_Texture;
struct SDL_Renderer;
struct TTF_Font;

namespace pgn {

    struct Glyph {
        FloatRect uvRect;    // Pixel coordinates in the atlas
        glm::vec2 size;      // Visual size of the glyph pixels
        float advance;       // Horizontal distance to next character
        glm::vec2 bearing;   // Offset from baseline (x: left, y: top)
    };

    class Font : public RefCounted
    {
    public:
        Font(SDL_Renderer* renderer, const std::string& path, float ptsize);
        ~Font();

        const Glyph& getGlyph(char c) const;
        SDL_Texture* getAtlasTexture() const { return m_atlasTexture; }
        const glm::vec2 getAtlasSize() const { return m_atlasSize; }
        bool isValid() const                 { return m_font && m_atlasTexture; }
        float getLineHeight() const;
        float getAscender() const;

    private:
        bool generateAtlas(SDL_Renderer* renderer);

        TTF_Font* m_font = nullptr;
        SDL_Texture* m_atlasTexture = nullptr;
        glm::vec2 m_atlasSize;
        std::map<char, Glyph> m_glyphs;
    };
}