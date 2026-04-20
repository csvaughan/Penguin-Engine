#pragma once
#include "Assets/Texture.h"
#include "Renderable.h"
#include "Memory/Ref.h"

namespace pgn {

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

    private:
        WeakRef<Texture> m_texture;
        IntRect m_textureRect;
    };
}