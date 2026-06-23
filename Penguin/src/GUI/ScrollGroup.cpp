#include "pgpch.h"
#include "GUI/ScrollGroup.h"
#include "Renderer/Renderer.h"
#include "Events/InputEvents.h"

namespace pgn::GUI {

    ScrollGroup::ScrollGroup(UICanvas* canvas, UIElementID id, Vector2 pos, Vector2 size, const std::string& name, GUIElement* parent)
        : GUIElement(canvas, id ,pos, size, name, parent) {}

    void ScrollGroup::OnRender(float alpha) 
    {
        // Draw the background/frame of the scroll area
        // This is NOT affected by the scroll offset or the scissor
        // Renderer::Submit(m_frameSprite); 
    }

    void ScrollGroup::RenderChildren(float alpha)
    {
        // 1. Clip children to the ScrollGroup's visible area
        Renderer::PushScissor(GetGlobalBounds(), m_zIndex);

        // 2. Shift the coordinate system for all children
        Renderer::PushTransform(Matrix4::Translate({-m_scrollOffset.x, -m_scrollOffset.y, 0.0f}));

        // 3. Render children using the standard logic
        GUIElement::RenderChildren(alpha);

        // 4. Restore Renderer state
        Renderer::PopTransform();
        Renderer::PopScissor(m_zIndex);
    }

    

    bool ScrollGroup::OnEvent(Event& e)
    {
        if (!m_visible || !m_enabled) return false;

        EventDispatcher dispatcher(e);

        // 1. Handle the Mouse Wheel for scrolling (only if hovering over the scroll container)
        dispatcher.Dispatch<MouseScrolledEvent>([&](MouseScrolledEvent& ev) {
            if (Contains(ev.GetPosition()))
            {
                m_scrollOffset.y -= ev.GetYOffset() * 20.0f;
                ClampScroll();
                return true; // Block event from game camera zoom
            }
            return false;
        });

        // 2. Check if mouse movement is outside our window to protect hidden children
        bool outsideScrollWindow = false;
        dispatcher.Dispatch<MouseMovedEvent>([&](MouseMovedEvent& ev) {
            if (!Contains(ev.GetPosition())) 
            {
                outsideScrollWindow = true;
            }
            return false; // ALWAYS return false here so e.Handled remains clean for other root components
        });

        // 3. Call the base class ONLY if the event shouldn't be blocked from children
        if (!outsideScrollWindow) 
        {
            if (GUIElement::OnEvent(e)) return true;
        }

        return false;
    }

    void ScrollGroup::ClampScroll()
    {
        // Calculate total content height
        float maxH = 0.0f;
        for(auto& child : m_children) {
            float bottom = child->GetTransform().position.y + child->GetLocalSize().y;
            if (bottom > maxH) maxH = bottom;
        }

        // Don't scroll past the top
        if (m_scrollOffset.y < 0) m_scrollOffset.y = 0;

        // Don't scroll past the bottom (if content is taller than the window)
        float windowHeight = GetLocalSize().y;
        if (maxH > windowHeight) {
            if (m_scrollOffset.y > maxH - windowHeight) 
                m_scrollOffset.y = maxH - windowHeight;
        } else {
            m_scrollOffset.y = 0; // Content fits, reset to top
        }
    }
}