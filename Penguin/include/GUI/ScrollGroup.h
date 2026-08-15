#pragma once
#include "GUI/GUIElement.h"

namespace pgn::GUI {

    class ScrollGroup : public GUIElement 
    {
    public:
        ScrollGroup(UICanvas* canvas, UIElementID id, Vector2 pos, Vector2 size, const std::string& name = "ScrollGroup", GUIElement* parent = nullptr);

        void SetScrollOffset(Vector2 offset) { m_scrollOffset = offset; }
        Vector2 GetScrollOffset() const { return m_scrollOffset; }

        // We override this so children of the scroll group know they are offset
        Vector2 ScreenToLocal(Vector2 screenPoint) const override 
        {
            Vector2 baseLocal = GUIElement::ScreenToLocal(screenPoint);
            return baseLocal + m_scrollOffset; // Add offset because content moved UP
        }

        virtual Vector2 GetInternalOffset() const override { return m_scrollOffset; }

    protected:
        virtual void OnRender(float alpha, Renderer& renderer) override;
        virtual void RenderChildren(float alpha, Renderer& renderer) override;
        virtual bool OnEvent(Event& e) override;

        void ClampScroll();

    private:
        Vector2 m_scrollOffset = { 0, 0 };
    };
}