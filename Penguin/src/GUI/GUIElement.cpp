#include "pgpch.h"
#include "GUI/GUIElement.h"
#include "GUI/UICanvas.h"

namespace pgn::GUI {

    std::atomic<UIElementID> GUIElement::s_idCounter{1};

    GUIElement::GUIElement(UICanvas* canvas, Vector2 pos, Vector2 size, const std::string& name, GUIElement* parent)
        : m_canvas(canvas), m_parent(parent), m_id(s_idCounter++), m_bounds({pos, size}), m_name(name)
    {
        m_transform.position = pos;
    }

    GUIElement::~GUIElement() = default;

    void GUIElement::InvalidateTransform() 
    {
        m_dirty = true;
        for (auto& child : m_children) child->InvalidateTransform();
    }

    AppContext &GUIElement::GetContext() { return m_canvas->m_context; }

    void GUIElement::RemoveChild(UIElementID id)
    {
        std::erase_if(m_children, [id](const Ref<GUIElement>& child) { return child->GetID() == id; });
        m_needsSort = true;
    }

    Vector2 GUIElement::ScreenToLocal(Vector2 screenPoint) const
    {
        if (m_parent) {
            // 1. Get the point in the Parent's internal space
            Vector2 pointOnParent = m_parent->ScreenToLocal(screenPoint);
            
            // 2. Subtract our position (relative to parent) and account for parent's scroll
            // Note: Subtracting internal offset here effectively 'scrolls' the mouse 
            // in the opposite direction of the visuals.
            return pointOnParent - m_transform.position - m_parent->GetInternalOffset();
        }
        
        // Root Case (Canvas): Account for Canvas Screen Pos and Scale
        return screenPoint / m_canvas->GetScaleFactor();
    }

    Vector2 GUIElement::GetScreenPos() const
    {
        if (!m_dirty) return m_cachedScreenPos;

        float scale = m_canvas->GetScaleFactor();
        Vector2 parentScreenPos = m_parent ? m_parent->GetScreenPos() : Vector2{0.0f, 0.0f};
        Vector2 parentRefSize = m_parent ? m_parent->GetLocalSize() : (m_canvas->GetSize() / scale);

        // 1. Calculate Anchor Point (Point on Parent)
        Vector2 anchorRefOff{0.0f, 0.0f};
        switch (m_anchor) 
        {
            case Anchor::TopLeft:      anchorRefOff = { 0.0f, 0.0f }; break;
            case Anchor::TopCenter:    anchorRefOff = { parentRefSize.x * 0.5f, 0.0f }; break;
            case Anchor::TopRight:     anchorRefOff = { parentRefSize.x, 0.0f }; break;
            case Anchor::CenterLeft:   anchorRefOff = { 0.0f, parentRefSize.y * 0.5f }; break;
            case Anchor::Center:       anchorRefOff = parentRefSize * 0.5f; break;
            case Anchor::CenterRight:  anchorRefOff = { parentRefSize.x, parentRefSize.y * 0.5f }; break;
            case Anchor::BottomLeft:   anchorRefOff = { 0.0f, parentRefSize.y }; break;
            case Anchor::BottomCenter: anchorRefOff = { parentRefSize.x * 0.5f, parentRefSize.y }; break;
            case Anchor::BottomRight:  anchorRefOff = parentRefSize; break;
        }

        // 2. Calculate Pivot Point (Point on Self)
        Vector2 pivotRefOff{0.0f, 0.0f};
        Vector2 mySize = GetLocalSize();
        switch (m_pivot)
        {
            case Pivot::TopLeft:     pivotRefOff = { 0.0f, 0.0f }; break;
            case Pivot::Center:      pivotRefOff = mySize * 0.5f; break;
            case Pivot::BottomRight: pivotRefOff = mySize; break;
        }

        Vector2 localRefOffset = anchorRefOff + m_transform.position - pivotRefOff;
        
        m_cachedScreenPos = parentScreenPos + (localRefOffset * scale);
        m_dirty = false;
        
        return m_cachedScreenPos;
    }

    FloatRect GUIElement::GetGlobalBounds() const 
    {
        Vector2 topLeft = GetScreenPos(); 
        float scale = m_canvas->GetScaleFactor();
        
        return { { topLeft.x, topLeft.y }, { m_bounds.w * scale, m_bounds.h * scale } };
    }

    void GUIElement::OnUpdate(Timestep dt) 
    {
        if (!m_enabled) return;
        if (m_needsSort) SortChildren();
        for (auto& child : m_children) child->OnUpdate(dt);
    }

    bool GUIElement::OnEvent(Event& e) 
    {
        if (!m_enabled || !m_visible) return false;
        for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
            if ((*it)->OnEvent(e)) return true;
        return false;
    }

    void GUIElement::SortChildren() 
    {
        std::stable_sort(m_children.begin(), m_children.end(), 
            [](const auto& a, const auto& b) { return a->m_zIndex < b->m_zIndex; });
        m_needsSort = false;
    }

    void GUIElement::SetZIndex(int z) 
    {
        if (m_zIndex == z) return;
        m_zIndex = z;
        if (m_parent) m_parent->m_needsSort = true;
        else m_canvas->m_needsSort = true;
    }
}