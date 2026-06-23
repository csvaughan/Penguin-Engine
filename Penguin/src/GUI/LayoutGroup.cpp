#include "pgpch.h"
#include "GUI/LayoutGroup.h"

namespace pgn::GUI {

    LayoutGroup::LayoutGroup(UICanvas* canvas, UIElementID id, Vector2 pos, Vector2 size, LayoutDirection direction, const std::string& name, GUIElement* parent)
        : GUIElement(canvas, id ,pos, size, name, parent), m_direction(direction)
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

            Vector2 childSize = child->GetLocalSize();
            Pivot childPivot = child->GetPivot(); 

            // 1. Determine the top-left slot position dictated by the layout flow
            Vector2 slotPos{ 0.0f, 0.0f };
            if (m_direction == LayoutDirection::Vertical) 
            {
                slotPos = { m_padding, currentY };
                currentY += childSize.y + m_spacing;
                maxCrossSize = std::max(maxCrossSize, childSize.x);
            } 
            else // Horizontal
            {
                slotPos = { currentX, m_padding };
                currentX += childSize.x + m_spacing;
                maxCrossSize = std::max(maxCrossSize, childSize.y);
            }

            // 2. Adjust the slot position based on the child's actual pivot!
            Vector2 pivotOffset{ 0.0f, 0.0f };
            switch (childPivot)
            {
                case Pivot::TopLeft:
                    pivotOffset = { 0.0f, 0.0f };
                    break;
                case Pivot::Center:
                    pivotOffset = childSize * 0.5f;
                    break;
                case Pivot::BottomRight:
                    pivotOffset = childSize;
                    break;
            }

            // 3. Force Anchor to TopLeft so layout calculations remain predictable relative to this container,
            // but preserve and apply the native pivot calculation!
            child->SetAnchor(Anchor::TopLeft);
            child->SetPosition(slotPos + pivotOffset);
        }

        // --- Apply Content Fitting ---
        if (m_fitWidth || m_fitHeight)
        {
            Vector2 newSize = GetLocalSize();

            if (m_direction == LayoutDirection::Vertical)
            {
                if (m_fitHeight) newSize.y = (m_children.empty() ? m_padding : currentY - m_spacing) + m_padding;
                if (m_fitWidth)  newSize.x = maxCrossSize + (m_padding * 2.0f);
            }
            else // Horizontal
            {
                if (m_fitWidth)  newSize.x = (m_children.empty() ? m_padding : currentX - m_spacing) + m_padding;
                if (m_fitHeight) newSize.y = maxCrossSize + (m_padding * 2.0f);
            }

            SetSize(newSize);
        }
    }
}