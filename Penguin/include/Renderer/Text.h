#pragma once
#include "Renderer/VertexArray.h"
#include "Math/Rect.h"
#include "Renderable.h"
#include "Memory/Ref.h"

struct SDL_Texture;

namespace pgn {

    class Font;

    class Text : public Renderable
    {
    public:
        Text() = default;
        Text(Ref<Font> font, const std::string& string = "", unsigned int characterSize = 30);

        // --- Setters ---
        void setString(const std::string& string) { m_content = string; m_isDirty = true; }
        void setFont(Ref<Font> font);
        void setLetterSpacing(float factor)       { m_letterSpacingFactor = factor; m_isDirty = true; }
        void setLineSpacing(float factor)         { m_lineSpacingFactor = factor;   m_isDirty = true; }
        void setMaxLineWidth(float width)         { m_maxLineWidth = width;         m_isDirty = true; }
        void setColor(Color c) override           { Renderable::setColor(c); m_isDirty = true; }
        void setOpacity(float o) override         { Renderable::setOpacity(o); m_isDirty = true; }

        // --- Getters ---
        const std::string& getString() const      { return m_content; }
        float getLetterSpacing() const            { return m_letterSpacingFactor; }
        float getLineSpacing() const              { return m_lineSpacingFactor; }
        float getMaxLineWidth() const             { return m_maxLineWidth; }

        const Ref<Font> getFont() const;
        const std::vector<Vertex>& getVertices() const      { if (m_isDirty) { recalculateGeometry(); } return m_vertices; }
        const std::vector<int>& getIndices() const          { if (m_isDirty) { recalculateGeometry(); } return m_indices; }
        SDL_Texture* getAtlasTexture() const;

        // --- Bounds Logic ---
        FloatRect getLocalBounds() const override;

    private:
        void recalculateGeometry() const;

    private:
        WeakRef<Font> m_font;
        std::string m_content;
        float m_letterSpacingFactor = 1.0f;
        float m_lineSpacingFactor = 1.0f;
        float m_maxLineWidth = 0.0f;

        // --- Cache Members ---
        mutable bool m_isDirty = true;
        mutable std::vector<Vertex> m_vertices;
        mutable std::vector<int> m_indices;
    };
}