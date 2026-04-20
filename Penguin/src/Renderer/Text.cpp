#include "Renderer/Text.h"
#include "Assets/Font.h"
#include <glm/vec2.hpp>


namespace pgn
{

    Text::Text(Ref<Font> font, const std::string &string, unsigned int characterSize): m_font(font), m_content(string) {}

    const Ref<Font> Text::getFont() const { return m_font.lock(); }


        void Text::setFont(Ref<Font> font) { m_font = font; m_isDirty = true; }

    SDL_Texture *Text::getAtlasTexture() const

    {
        auto font = m_font.lock(); 
        return font ? font->getAtlasTexture() : nullptr;
    }

    FloatRect Text::getLocalBounds() const 
    {
        if (m_isDirty) { recalculateGeometry(); }
        if (m_vertices.empty()) return { 0, 0, 0, 0 };

        float minX = m_vertices[0].position.x;
        float minY = m_vertices[0].position.y;
        float maxX = m_vertices[0].position.x;
        float maxY = m_vertices[0].position.y;

        for (const auto& v : m_vertices) 
        {
            minX = std::min(minX, v.position.x);
            minY = std::min(minY, v.position.y);
            maxX = std::max(maxX, v.position.x);
            maxY = std::max(maxY, v.position.y);
        }

        return { minX, minY, maxX - minX, maxY - minY };
    }

    void Text::recalculateGeometry() const
    {
        m_vertices.clear();
        m_indices.clear();
        m_isDirty = false;

        auto font = m_font.lock();
        if (!font || m_content.empty()) return;

        glm::vec2 texSize = font->getAtlasSize();
        float cursorX = 0.0f;
        
        // Use the Ascender so that the top of the line is at y=0
        float cursorY = font->getAscender(); 
        float lineHeight = font->getLineHeight() * m_lineSpacingFactor;

        for (char c : m_content) 
        {
            // 1. Handle Newlines
            if (c == '\n') {
                cursorX = 0;
                cursorY += lineHeight;
                continue;
            }

            const Glyph& glyph = font->getGlyph(c);

            // 2. Wrap Logic
            if (m_maxLineWidth > 0 && (cursorX + glyph.advance) > m_maxLineWidth && c != ' ') 
            {
                cursorX = 0;
                cursorY += lineHeight;
            }

            // 3. Generate Geometry (Skip for invisible characters like spaces)
            if (glyph.size.x > 0 && glyph.size.y > 0) 
            {
                float x = cursorX + glyph.bearing.x;
                float y = cursorY - glyph.bearing.y; 
                float w = glyph.size.x;
                float h = glyph.size.y;

                float u1 = glyph.uvRect.x / texSize.x;
                float v1 = glyph.uvRect.y / texSize.y;
                float u2 = (glyph.uvRect.x + glyph.uvRect.w) / texSize.x;
                float v2 = (glyph.uvRect.y + glyph.uvRect.h) / texSize.y;

                int base = (int)m_vertices.size();

                // Note: Use getColor() and getOpacity() logic if your Vertex supports it
                m_vertices.push_back(Vertex{{x, y},         getColor(), {u1, v1}}); // TL
                m_vertices.push_back(Vertex{{x + w, y},     getColor(), {u2, v1}}); // TR
                m_vertices.push_back(Vertex{{x + w, y + h}, getColor(), {u2, v2}}); // BR
                m_vertices.push_back(Vertex{{x, y + h},     getColor(), {u1, v2}}); // BL

                m_indices.push_back(base + 0); m_indices.push_back(base + 1); m_indices.push_back(base + 2);
                m_indices.push_back(base + 2); m_indices.push_back(base + 3); m_indices.push_back(base + 0);
            }

            // Always advance the cursor, even for spaces
            cursorX += glyph.advance * m_letterSpacingFactor;
        }
    }
} // namespace pgn