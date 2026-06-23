#include "GUI/Panel.h"
#include "GUI/UICanvas.h"
#include "GUI/UIImage.h"
#include "Renderer/Renderer.h"
#include "Events/InputEvents.h"

namespace pgn::GUI {

    Panel::Panel(UICanvas* canvas, UIElementID id, Vector2 pos, Vector2 size, const std::string& name, GUIElement* parent) 
        : GUIElement(canvas, id ,pos, size, name, parent), m_background({pos, size}) 
        {}

    void Panel::OnUpdate(Timestep ts) 
    {
        if (!m_enabled) return;
        GUIElement::OnUpdate(ts);
    }

    void Panel::SetTexture(Ref<Texture> texture)
    {
        if(m_image)
            m_image->SetImage(texture);
        else
            m_image = AddChild<UIImage>(Vector2{0,0}, m_bounds.getSize(), texture, "PanelImage");
    }

    void Panel::OnRender(float alpha) 
    {
        if (!m_visible) return;
        m_backgroundColor.a = (!m_enabled) ? 0.5f : 1.0f;
            
        auto bounds = GetGlobalBounds(); 

        if (m_image) 
        {
            m_image->SetZIndex(m_zIndex + 1); 
            m_image->SetColor(m_backgroundColor);
        }
        else
        {
            m_background.setSize(bounds.getSize());
            m_background.setPosition(bounds.getPosition());
            m_background.setZIndex(m_zIndex); 
            m_background.setColor(m_backgroundColor);
            Renderer::Submit(m_background);
        }

        GUIElement::OnRender(alpha);
    }

    bool Panel::OnEvent(Event& e) 
    {
        if (!m_visible || !m_enabled) return false;
        if (GUIElement::OnEvent(e)) return true;

        EventDispatcher dispatcher(e);
        
        // The dispatcher modifies e.Handled under the hood if the lambda returns true
        dispatcher.Dispatch<MouseButtonPressedEvent>([this](auto& ev) {
            return m_isOpaque && Contains(ev.GetPosition());
        });

        return e.Handled; 
    }
}