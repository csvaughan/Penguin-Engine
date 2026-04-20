#include "pgpch.h"
#include "GUI/LayoutGroup.h"

namespace pgn::GUI {

    LayoutGroup::LayoutGroup(UICanvas* canvas, Vector2 pos, Vector2 size, LayoutDirection direction, GUIElement* parent, const std::string& name)
        : GUIElement(canvas, pos, size, name, parent), m_direction(direction)
    {
    }

    void LayoutGroup::SetDirection(LayoutDirection direction) 
    {
        if (m_direction == direction) return;
        m_direction = direction;
        m_layoutDirty = true;
    }

    void LayoutGroup::SetSpacing(float spacing) 
    {
        if (m_spacing == spacing) return;
        m_spacing = spacing;
        m_layoutDirty = true;
    }

    void LayoutGroup::SetPadding(float padding) 
    {
        if (m_padding == padding) return;
        m_padding = padding;
        m_layoutDirty = true;
    }

    void LayoutGroup::SetFitContent(bool fitWidth, bool fitHeight)
    {
        if (m_fitWidth == fitWidth && m_fitHeight == fitHeight) return;
        m_fitWidth = fitWidth;
        m_fitHeight = fitHeight;
        m_layoutDirty = true;
    }

    void LayoutGroup::OnUpdate(Timestep dt) 
    {
        if (m_layoutDirty || m_needsSort) 
        {
            RecalculateLayout();
            m_layoutDirty = false;
        }

        GUIElement::OnUpdate(dt);
    }

    void LayoutGroup::RecalculateLayout() 
    {
        float currentX = m_padding;
        float currentY = m_padding;
        float maxCrossSize = 0.0f; // Tracks the widest child in a vertical layout, or tallest in horizontal

        for (auto& child : m_children) 
        {
            if (!child->IsVisible()) continue;

            child->SetAnchor(Anchor::TopLeft);
            child->SetPivot(Pivot::TopLeft);
            Vector2 childSize = child->GetLocalSize();

            if (m_direction == LayoutDirection::Vertical) 
            {
                child->SetPosition({ m_padding, currentY });
                currentY += childSize.y + m_spacing;
                maxCrossSize = std::max(maxCrossSize, childSize.x);
            } 
            else // Horizontal
            {
                child->SetPosition({ currentX, m_padding });
                currentX += childSize.x + m_spacing;
                maxCrossSize = std::max(maxCrossSize, childSize.y);
            }
        }

        // --- Apply Content Fitting ---
        if (m_fitWidth || m_fitHeight)
        {
            Vector2 newSize = GetLocalSize();

            if (m_direction == LayoutDirection::Vertical)
            {
                // Subtract the trailing spacing from the last child, add bottom padding
                if (m_fitHeight) newSize.y = (m_children.empty() ? m_padding : currentY - m_spacing) + m_padding;
                if (m_fitWidth)  newSize.x = maxCrossSize + (m_padding * 2.0f);
            }
            else // Horizontal
            {
                if (m_fitWidth)  newSize.x = (m_children.empty() ? m_padding : currentX - m_spacing) + m_padding;
                if (m_fitHeight) newSize.y = maxCrossSize + (m_padding * 2.0f);
            }

            // Calling SetSize will automatically trigger InvalidateTransform()
            // which updates the screen positions for this element and its children.
            SetSize(newSize);
        }
    }
}