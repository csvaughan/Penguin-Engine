#pragma once
#include "Label.h"
#include "Panel.h"
#include "Renderer/Text.h"
#include <functional>

namespace pgn::GUI {

    class Button : public Panel
    {
    public:

        Button(UICanvas* canvas, UIElementID id, Vector2 pos, Vector2 size, const std::string& name = "Button", GUIElement* parent = nullptr);

        void SetCallback(std::function<void()> callback) { m_onClick = callback; }

        void SetBackgroundColor(const Color color) override;
        void SetHoverColor(const Color color) { m_hoverColor = color; m_autoColor = false; }
        void SetPressedColor(const Color color) { m_pressedColor = color; m_autoColor = false; }
        
        void SetTextSize(float size) { if(m_text) m_text->SetFontSize(size); }
        void SetText(const std::string& text);  
        void SetText(const std::string& text, Ref<Font> font);

    private:
        
        void DefaultFunction() { std::println("{} pressed!", m_name); }

        void OnUpdate(Timestep ts) override;
        bool OnEvent(Event& e) override;

        using GUIElement::AddChild;

        void UpdateAppearance();

    private:
        std::function<void()> m_onClick;

        Label* m_text = nullptr;

        Color m_baseColor; 
        Color m_hoverColor;   
        Color m_pressedColor; 

        bool m_autoColor = true;
        bool m_isHovered = false;
        bool m_isPressed = false;
    };
}