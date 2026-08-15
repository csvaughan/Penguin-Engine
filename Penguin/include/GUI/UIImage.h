#pragma once
#include "GUI/GUIElement.h"
#include "Renderer/Sprite.h"
#include "Renderer/Renderer.h"
#include "UICanvas.h"

namespace pgn::GUI
{
    class UIImage: public GUIElement
    {
    public:
        UIImage(UICanvas* canvas, UIElementID id, Vector2 position, Vector2 size, Ref<Texture> texture, const std::string& name = "Image", GUIElement* parent = nullptr) 
            : GUIElement(canvas, id, position, size, name, parent)
        {
            SetImage(texture);
        }
        ~UIImage() {}

        void SetImage(Ref<Texture> texture) 
        {
            m_sprite.setTexture(texture); 
            
            InvalidateTransform();
        }

        void SetColor(Color color) { m_sprite.setColor(color); }

    private:
        void OnUpdate(Timestep ts) override
        {
            GUIElement::OnUpdate(ts);
        }

        void OnRender(float alpha, Renderer& renderer) override
        {
            if (!m_enabled) return;

            FloatRect globalBounds = GetGlobalBounds();
            
            m_sprite.setPosition(globalBounds.getPosition());
            Vector2 texSize = m_sprite.getTextureRect().getSize();
            m_sprite.setScale({ globalBounds.w / texSize.x, globalBounds.h / texSize.y });
            m_sprite.render(renderer);
        }

    private:
        Sprite m_sprite;
    };
} // namespace pgn::GUI