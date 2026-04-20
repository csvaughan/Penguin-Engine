#pragma once
#include "GUIElement.h"
#include "Renderer/Text.h"
#include "Assets/Font.h"

namespace pgn::GUI {

    class Label : public GUIElement
    {
    public:
        Label(UICanvas* canvas, Vector2 position, Vector2 size,  const std::string& text, Ref<Font> font, GUIElement* parent = nullptr, const std::string& name = "Label");

        void SetText(const std::string& text);
        void SetColor(const Color& color) { m_text.setColor(color); }

    private:
        // Standard overrides
        virtual void OnUpdate(Timestep ts) override;
        virtual void OnRender(float alpha) override;
        
    private:
        Text m_text;
    };

} // namespace pgn::GUI