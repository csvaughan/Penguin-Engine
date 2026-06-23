#include "GUI/Button.h"
#include "GUI/UICanvas.h"
#include "Events/InputEvents.h"
#include "Renderer/Renderer.h"

namespace pgn::GUI {

    Button::Button(UICanvas* canvas, UIElementID id, Vector2 pos, Vector2 size, const std::string& name, GUIElement* parent)
        : Panel(canvas, id ,pos, size, name, parent) 
    {
        SetBackgroundColor(m_backgroundColor);       
    }

    void Button::SetText(const std::string &text, Ref<Font> font)
    {
        if(m_text)
        {
            m_text->SetText(text);
            m_text->SetFont(font);
        }
        else
        {
            m_text = AddChild<Label>(Vector2{0,0}, m_bounds.getSize(), text, font, "ButtonText");
            m_text->SetZIndex(GetZIndex() + 1);
        }  
    }

    void pgn::GUI::Button::SetText(const std::string &text)
    {
        if(m_text) m_text->SetText(text); 
    }

    void Button::SetBackgroundColor(const Color color)
    {
        m_baseColor = color;
        if (m_autoColor)
        {
            m_hoverColor = color * 0.8f;   
            m_pressedColor = color * 0.5f; 
            
            // Re-clamp alpha 
            m_hoverColor.a = color.a;
            m_pressedColor.a = color.a;
        }
    }

    void Button::UpdateAppearance()
    {
        if(m_text) m_text->SetZIndex(GetZIndex() + 1);

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
            {
                if (m_onClick) m_onClick(); 
                else DefaultFunction();
            }
            
            m_isPressed = false;
            return inside; 
        });

        return false;
    }
}