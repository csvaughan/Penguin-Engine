#pragma once
#include "GUIElement.h"
#include "Renderer/RenderShape.h"

namespace pgn::GUI {

    class Panel : public GUIElement
    {
    public:
        Panel(UICanvas* canvas, Vector2 pos, Vector2 size, GUIElement* parent = nullptr, const std::string& name = "Panel");

        void SetBackgroundColor(const Color& color) { m_backgroundColor = color; }
        void SetOpacity(float opacity) { m_backgroundColor.a = opacity; }

        virtual void OnUpdate(Timestep ts) override;
        virtual void OnRender(float alpha) override;
        virtual bool OnEvent(Event& e) override;

    protected:
        RectangleShape m_background;
        Color m_backgroundColor = Color::Grey;
        bool m_isOpaque = true; 
    };
}