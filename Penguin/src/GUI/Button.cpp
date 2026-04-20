#include "GUI/Button.h"
#include "GUI/Label.h"
#include "GUI/UICanvas.h"
#include "Events/InputEvents.h"

namespace pgn::GUI {

    Button::Button(UICanvas* canvas, Vector2 pos, Vector2 size, const std::string& text, GUIElement* parent, const std::string& name)
        : Panel(canvas, pos, size, parent, name) 
    {
        // Set the initial colors and auto-generate hover/press variants
        SetBaseColor(m_backgroundColor);        
        auto l = AddChild<Label>(Vector2{0,0}, size, ((text.empty()) ? m_name : text), GetContext().GetFont("default_font"));
    }

    void Button::SetBaseColor(const Color& color)
    {
        m_baseColor = color;
        if (m_autoColor)
        {
            m_hoverColor = color * 0.8f;   
            m_pressedColor = color * 0.8f; 
            
            // Re-clamp alpha (multiplication shouldn't affect transparency usually)
            m_hoverColor.a = color.a;
            m_pressedColor.a = color.a;
        }
    }

    void Button::UpdateAppearance()
    {
        if (m_isPressed)      m_backgroundColor = m_pressedColor;
        else if (m_isHovered) m_backgroundColor = m_hoverColor;
        else                  m_backgroundColor = m_baseColor;
    }

    void Button::OnUpdate(Timestep ts) 
    {
        if (!m_enabled) return;

        UpdateAppearance(); // Sync the background color based on current state
        Panel::OnUpdate(ts); // Proceed with Panel's position syncing and child updates
    }

    bool Button::OnEvent(Event& e) 
    {
        if (!m_visible || !m_enabled) return false;

        // 1. Children get priority (e.g. a checkbox inside a button)
        if (GUIElement::OnEvent(e)) return true;

        EventDispatcher dispatcher(e);

        // Handle Hover
        dispatcher.Dispatch<MouseMovedEvent>([&](MouseMovedEvent& ev) {
            if (!ev.Handled)
            {
                if(Contains(ev.GetPosition())) 
                {
                    m_isHovered = true;
                }
                else
                {
                    m_isHovered = false;
                    m_isPressed = false;
                }
            }
                
            return false; 
        });

        // Handle Press
        dispatcher.Dispatch<MouseButtonPressedEvent>([&](MouseButtonPressedEvent& ev) {
            if (Contains(ev.GetPosition())) 
            {
                m_isPressed = true;
                return true;
            }
            return false;
        });

        // Handle Release
        dispatcher.Dispatch<MouseButtonReleasedEvent>([&](MouseButtonReleasedEvent& ev) {
            bool inside = Contains(ev.GetPosition());
            
            if (m_isPressed && inside) 
                if (m_onClick) m_onClick(); 
            
            m_isPressed = false;
            return inside; 
        });

        return false;
    }
}