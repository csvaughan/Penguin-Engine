#include "GUI/MenuBox.h"

namespace pgn::GUI {

    MenuBox::MenuBox(UICanvas* canvas, UIElementID id, Vector2 pos, Vector2 size, const std::string& name, GUIElement* parent)
        : Panel(canvas, id, pos, size, name, parent)
    {
        SetBackgroundColor(Color{ 35, 35, 35, 255 });

        // Match your specific constructor signatures by providing a default ID (0)
        // AddChild automatically appends 'this' as the parent pointer slot
        m_scrollGroup = AddChild<ScrollGroup>(Vector2{ 0.0f, 0.0f }, size, "MenuScrollGroup");

        m_layoutGroup = m_scrollGroup->AddChild<LayoutGroup>(Vector2{ 0.0f, 0.0f }, size, LayoutDirection::Vertical, "MenuLayoutGroup");

        m_layoutGroup->SetFitContent(false, true);
        m_layoutGroup->SetPadding(10.0f);
        m_layoutGroup->SetSpacing(15.0f);
    }

} // namespace pgn::GUI