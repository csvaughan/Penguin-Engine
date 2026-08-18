#pragma once
#include "Texture.h"
#include "Renderable.h"

namespace pgn {

    class Renderer;

    class Sprite : public Renderable
    {
    public:
        Sprite() = default;

        void setTexture(Ref<Texture> tex, bool resetRect = true) 
        {
            m_texture = tex;
            if (tex && resetRect) 
                m_textureRect = { 0, 0, static_cast<int>(tex->getWidth()), static_cast<int>(tex->getHeight()) };
        }

        void setTextureRect(const IntRect& rect) { m_textureRect = rect; }

        // Getters for the Renderer
        WeakRef<Texture> getTexture() const { return m_texture; }
        const IntRect& getTextureRect() const { return m_textureRect; }
        FloatRect getLocalBounds() const override { return { 0.0f, 0.0f, static_cast<float>(m_textureRect.w), static_cast<float>(m_textureRect.h) }; }

        void render(Renderer& renderer) const override;

    private:
        WeakRef<Texture> m_texture;
        IntRect m_textureRect;
    };
}