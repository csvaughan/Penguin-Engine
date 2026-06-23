#pragma once
#include "Renderer/RenderShape.h"
#include "GUIElement.h"

namespace pgn::GUI {

    class UIImage;

    class Panel : public GUIElement
    {
    public:
        Panel(UICanvas* canvas, UIElementID id, Vector2 pos, Vector2 size, const std::string& name = "Panel", GUIElement* parent = nullptr);

        virtual void SetBackgroundColor(const Color color) { m_backgroundColor = color; }
        void SetOpacity(float opacity) { m_backgroundColor.a = opacity; }
        void SetTexture(Ref<Texture> texture);

        virtual void OnUpdate(Timestep ts) override;
        virtual void OnRender(float alpha) override;
        virtual bool OnEvent(Event& e) override;

    protected:
        
        UIImage* m_image = nullptr;
        RectangleShape m_background;
        Color m_backgroundColor = Color::White;
        bool m_isOpaque = true; 
    };
}