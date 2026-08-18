#pragma once
#include "Math/Rect.h"
#include "Math/Vector.h"
#include "Renderer/Texture.h"

namespace pgn
{
    struct Glyph {
        FloatRect uvRect;
        Vector2 size;
        float advance;
        Vector2 bearing;
    };

    class Font : public IAsset
    {
    public:
        Font(SDL_Renderer* renderer, const std::string& path, float ptsize);
        ~Font() = default;

        float getLineHeight() const { return m_lineHeight; }
        float getAscender() const   { return m_ascender; }
        
        Ref<Texture> getAtlasTexture() const { return m_atlasTexture; } 
        Vector2 getAtlasSize() const         { return m_atlasSize; }
        const Glyph& getGlyph(char c) const;

    private:
        bool generateAtlas(SDL_Renderer* renderer, const std::string& path, float ptsize);

        Ref<Texture> m_atlasTexture = nullptr; 
        Vector2 m_atlasSize = { 0.f, 0.f };
        std::unordered_map<char, Glyph> m_glyphs;
        
        float m_lineHeight = 0.0f;
        float m_ascender = 0.0f;
    };
}