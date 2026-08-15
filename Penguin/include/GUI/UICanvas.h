#pragma once
#include "Math/Vector.h"
#include "Memory/Ref.h"
#include "Core/Timestep.h"

namespace pgn {class Event; class Renderer; }

namespace pgn::GUI {

    class GUIElement;

    using UIElementID = size_t;

    class UICanvas 
    {
    public:
        UICanvas(Vector2 size);
        ~UICanvas() = default;

        template<typename TElement, typename... Args>
        requires(std::is_base_of_v<GUIElement, TElement>)
        TElement* CreateElement(Args&&... args)
        {
            auto element = CreateRef<TElement>(this, m_currentId++, std::forward<Args>(args)...);
            
            m_rootElements.push_back(element);
            m_idMap[element->GetID()] = WeakRef<GUIElement>(element);

            SortRootElements();
            return element.get();
        }

        GUIElement* FindElement(UIElementID id) { return (m_idMap.contains(id)) ? m_idMap.at(id).lock().get() : nullptr; }

        void RemoveElement(UIElementID id);

        void OnUpdate(Timestep dt);
        void OnRender(float alpha, Renderer& renderer);
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

        UIElementID m_currentId;
        bool m_needsSort = false;
        bool m_isEnabled = true;
        bool m_isEventBlocking = false;
        Vector2 m_size;
        Vector2 m_referenceRes; // Baseline resolution (e.g. 1080p)

        std::vector<Ref<GUIElement>> m_rootElements;
        std::unordered_map<UIElementID, WeakRef<GUIElement>> m_idMap;

        friend class GUIElement;
    };

} // namespace pgn::GUI