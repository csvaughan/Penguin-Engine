#include "GUI/Panel.h"
#include "GUI/UICanvas.h"
#include "Renderer/Renderer.h"
#include "Events/InputEvents.h"

namespace pgn::GUI {

    Panel::Panel(UICanvas* canvas, Vector2 pos, Vector2 size, GUIElement* parent, const std::string& name) 
        : GUIElement(canvas, pos, size, name, parent), m_background({pos, size}) {}

    void Panel::OnUpdate(Timestep ts) 
    {
        if (!m_enabled) return;
        GUIElement::OnUpdate(ts);
    }

    void Panel::OnRender(float alpha) 
    {
        if (!m_visible) return;
        m_backgroundColor.a = (!m_enabled) ? .5f : 1.0f;
            
        auto bounds = GetGlobalBounds(); 
        m_background.setPosition(bounds.getPosition());
        m_background.setSize(bounds.getSize());
        m_background.setColor(m_backgroundColor);
        Renderer::Submit(m_background);
        GUIElement::OnRender(alpha);
    }

    bool Panel::OnEvent(Event& e) 
    {
        if (!m_visible || !m_enabled) return false;
        if (GUIElement::OnEvent(e)) return true;

        // Base Panel behavior: Consume mouse clicks if opaque
        EventDispatcher dispatcher(e);
        return dispatcher.Dispatch<MouseButtonPressedEvent>([this](auto& ev) {
            return m_isOpaque && Contains({ (float)ev.GetX(), (float)ev.GetY() });
        });
    }
}