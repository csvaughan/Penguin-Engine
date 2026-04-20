#pragma once
#include "GUIElement.h"
#include "Core/AppContext.h"

namespace pgn::GUI {

    class UICanvas 
    {
    public:
        UICanvas();
        ~UICanvas() = default;

        template<typename TElement, typename... Args>
        requires(std::is_base_of_v<GUIElement, TElement>)
        TElement* CreateElement(Args&&... args)
        {
            auto element = CreateRef<TElement>(this, std::forward<Args>(args)...);
            m_rootElements.push_back(element);
            SortRootElements();
            return element.get();
        }

        void RemoveElement(UIElementID id);

        void OnUpdate(Timestep dt);
        void OnRender(float alpha);
        bool OnEvent(Event& e);
        
        void SetSize(Vector2 size);
        void SetEnabled(bool enabled) { m_isEnabled = enabled; }
        void SetEventBlocking(bool blocking) { m_isEventBlocking = blocking; }

        float GetScaleFactor() const { return m_size.y / m_referenceRes.y; }
        Vector2 GetSize() const { return m_size; }
        bool IsEnabled() const { return m_isEnabled; }

    private:

        void BringToFront(UIElementID id);
        void SortRootElements();
        void CleanupDeadElements();

    private:
        AppContext m_context;
        bool m_needsSort = false;
        bool m_isEnabled = true;
        bool m_isEventBlocking = false;
        Vector2 m_size;
        Vector2 m_referenceRes; // Baseline resolution (e.g. 1080p)
        std::vector<Ref<GUIElement>> m_rootElements;

        friend class GUIElement;
    };

} // namespace pgn::GUI