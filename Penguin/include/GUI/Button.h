#pragma once
#include "Panel.h"
#include <functional>

namespace pgn::GUI {

    class Button : public Panel
    {
    public:

        Button(UICanvas* canvas, Vector2 pos, Vector2 size, const std::string& text = "", GUIElement* parent = nullptr, const std::string& name = "Button");

        void SetCallback(std::function<void()> callback) { m_onClick = callback; }

        void SetBaseColor(const Color& color);
        void SetHoverColor(const Color& color) { m_hoverColor = color; m_autoColor = false; }
        void SetPressedColor(const Color& color) { m_pressedColor = color; m_autoColor = false; }

    private:

        virtual void OnUpdate(Timestep ts) override;
        virtual bool OnEvent(Event& e) override;

        void UpdateAppearance();

    private:
        std::function<void()> m_onClick;

        Color m_baseColor{ 60, 60, 60, 255 };
        Color m_hoverColor{ 80, 80, 80, 255 };
        Color m_pressedColor{ 40, 40, 40, 255 };

        bool m_autoColor = true;
        bool m_isHovered = false;
        bool m_isPressed = false;
    };
}