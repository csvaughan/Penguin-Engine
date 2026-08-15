#include "GUI/Label.h"
#include "Renderer/Renderer.h"
#include "GUI/UICanvas.h"

namespace pgn::GUI {

   Label::Label(UICanvas* canvas, UIElementID id, Vector2 position, Vector2 size, const std::string& text, Ref<Font> font, const std::string& name, GUIElement* parent)
    : GUIElement(canvas, id ,position, size, name, parent)
    {
        m_text.setFont(font);
        m_text.setColor(Color::Black);
        SetText(text); 
    }

    void Label::SetText(const std::string& text)
    {
        m_text.setString(text);
        m_text.setScale(Vector2{1.0f, 1.0f} * m_canvas->GetScaleFactor());
        FloatRect bounds = m_text.getLocalBounds();
        m_text.setOrigin({ bounds.x + bounds.w * 0.5f, bounds.y + bounds.h * 0.5f });
        InvalidateTransform();
    }

    void Label::OnUpdate(Timestep ts)
    {
        if (!m_enabled) return;

        float scale = m_canvas->GetScaleFactor();
        
        float width = m_parent ? m_parent->GetLocalSize().x : m_bounds.w;
        float height = m_parent ? m_parent->GetLocalSize().y : m_bounds.h;
        Vector2 pos = m_parent ? m_parent->GetScreenPos() : GetScreenPos();

        Vector2 center = pos + Vector2{ (width * 0.5f) * scale, (height * 0.5f) * scale };
        m_text.setPosition(center);

        m_text.setScale({
            m_transform.scale.x * m_fontSize.x * scale,
            m_transform.scale.y * m_fontSize.y * scale
        });
    }

    void Label::OnRender(float alpha, Renderer& renderer)
    {
        if (!m_visible) return;
        m_text.render(renderer);
        GUIElement::OnRender(alpha, renderer);
    }

} // namespace pgn::GUI