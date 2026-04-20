#include "GUI/Label.h"
#include "Renderer/Renderer.h"
#include "GUI/UICanvas.h"

namespace pgn::GUI {

   Label::Label(UICanvas* canvas, Vector2 position, Vector2 size, const std::string& text, Ref<Font> font, GUIElement* parent, const std::string& name)
    : GUIElement(canvas, position, size, name, parent)
    {
        m_text.setFont(font);
        m_text.setColor(Color::Black);
        m_text.setScale(size * m_canvas->GetScaleFactor());
        SetText(text); 
    }

    void Label::SetText(const std::string& text)
    {
        m_text.setString(text);
        FloatRect bounds = m_text.getLocalBounds();
        m_text.setOrigin({ bounds.w * 0.5f, bounds.h * 0.5f });
        
        InvalidateTransform();
    }

    void Label::OnUpdate(Timestep ts)
    {
        if (!m_enabled) return;

        float scale = m_canvas->GetScaleFactor();
        Vector2 center = GetScreenPos() + Vector2{ (m_bounds.w * 0.5f) * scale, (m_bounds.h * 0.5f) * scale };
        m_text.setPosition(center);
        m_text.setScale(m_transform.scale * scale);

        GUIElement::OnUpdate(ts);
    }

    void Label::OnRender(float alpha)
    {
        if (!m_visible) return;

        Renderer::Submit(m_text);
        GUIElement::OnRender(alpha);
    }

} // namespace pgn::GUI