#pragma once
#include "GUIElement.h"
#include "Renderer/Text.h"
#include "Assets/Font.h"

namespace pgn::GUI {

    class Label : public GUIElement
    {
    public:
        Label(UICanvas* canvas, UIElementID id, Vector2 position, Vector2 size, const std::string& text, Ref<Font> font, const std::string& name = "Label", GUIElement* parent = nullptr);

        void SetText(const std::string& text);
        void SetTextColor(const Color& color) { m_text.setColor(color); }

        void SetFont(Ref<Font> font)     { m_text.setFont(font); }
        void SetFontSize(float size)     { m_fontSize = { size, size }; }
        void SetFontSize(Vector2 size)   { m_fontSize = size; }

        void SetZIndex(int z) override   { m_text.setZIndex(z); }

        Vector2 GetFontSize() const      { return m_fontSize; }
        int GetZIndex() const override   { return m_text.getZIndex(); } 

    private:
        void OnUpdate(Timestep ts) override;
        void OnRender(float alpha) override;
        bool OnEvent(Event& e) override { return false; } 
        
    private:
        Text m_text;
        Vector2 m_fontSize{ 1.0f, 1.0f };
    };

} // namespace pgn::GUI