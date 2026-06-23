#pragma once
#include "GUIElement.h"
#include "ScrollGroup.h"
#include "Panel.h"
#include "LayoutGroup.h"

namespace pgn::GUI {

    class MenuBox : public Panel
    {
    public:
        MenuBox(UICanvas* canvas, UIElementID id, Vector2 pos, Vector2 size, const std::string& name = "MenuBox", GUIElement* parent = nullptr);
        virtual ~MenuBox() = default;

        // Clean wrapper interface to append items directly into the layout pipeline
        template<typename TElement, typename... Args>
        requires(std::is_base_of_v<GUIElement, TElement>)
        TElement* AddMenuItem(Args&&... args)
        {
            // Direct the child allocations onto the layout group container underneath
            return m_layoutGroup->AddChild<TElement>(std::forward<Args>(args)...);
        }

        // Layout customization configuration passes
        void SetSpacing(float spacing) { m_layoutGroup->SetSpacing(spacing); }
        void SetPadding(float padding) { m_layoutGroup->SetPadding(padding); }

    private:
        // Internal structural building blocks
        ScrollGroup* m_scrollGroup = nullptr;
        LayoutGroup* m_layoutGroup = nullptr;
    };

} // namespace pgn::GUI