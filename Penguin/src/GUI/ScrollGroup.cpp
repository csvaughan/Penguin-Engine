#include "pgpch.h"
#include "GUI/ScrollGroup.h"
#include "Renderer/Renderer.h"
#include "Events/InputEvents.h"

namespace pgn::GUI {

    ScrollGroup::ScrollGroup(UICanvas* canvas, Vector2 pos, Vector2 size, GUIElement* parent, const std::string& name)
        : GUIElement(canvas, pos, size, name, parent) {}

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

        // 1. Handle the Mouse Wheel for scrolling
        dispatcher.Dispatch<MouseScrolledEvent>([&](MouseScrolledEvent& ev) {
            if (Contains(ev.GetPosition()))
            {
                // Update the Y offset (multiply by a speed factor, e.g., 20.0f)
                m_scrollOffset.y -= ev.GetYOffset() * 20.0f;
                // Optional: Clamp the scroll so you don't scroll into infinity
                // m_scrollOffset.y = Math::Clamp(m_scrollOffset.y, 0.0f, m_maxScrollHeight);
                
                return true; // Mark as handled so the camera doesn't zoom too
            }
            return false;
        });

        // 2. Handle Mouse Movement (Hover states, etc.)
        dispatcher.Dispatch<MouseMovedEvent>([&](MouseMovedEvent& ev) {
            // If the mouse is outside our visible window then stop the event from reaching children
            return (!Contains(ev.GetPosition())) ? true : false;
        });

        // 3. Important: Call the base class!
        if (GUIElement::OnEvent(e)) return true;

        return false;
    }

    void ScrollGroup::ClampScroll()
    {
        // Calculate total content height
        float maxH = 0.0f;
        for(auto& child : m_children) {
            float bottom = child->GetScreenPos().y + child->GetLocalSize().y;
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