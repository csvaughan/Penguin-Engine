#pragma once
#include "GUIElement.h"

namespace pgn::GUI {

    enum class LayoutDirection { Vertical, Horizontal };

    class LayoutGroup : public GUIElement 
    {
    public:
        LayoutGroup(UICanvas* canvas, Vector2 pos, Vector2 size, LayoutDirection direction, GUIElement* parent = nullptr, const std::string& name = "LayoutGroup");
        virtual ~LayoutGroup() = default;

        void SetDirection(LayoutDirection direction);
        void SetSpacing(float spacing);
        void SetPadding(float padding);
        void SetFitContent(bool fitWidth, bool fitHeight);

    protected:
        void OnUpdate(Timestep dt) override;

    private:
        void RecalculateLayout();

    private:
        LayoutDirection m_direction;
        float m_spacing = 5.0f;
        float m_padding = 5.0f;
        bool m_fitWidth = false;
        bool m_fitHeight = false;

        bool m_layoutDirty = true; 
    };
}