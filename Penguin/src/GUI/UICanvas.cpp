#include "pgpch.h"
#include "GUI/UICanvas.h"
#include "Events/InputEvents.h"
#include "Events/WindowEvents.h"
#include <algorithm>

namespace pgn::GUI {

    UICanvas::UICanvas() 
        : m_size(m_context.GetWindow().GetWindowSize()), 
          m_referenceRes(Vector2{1920.0f, 1080.0f}){}


    void pgn::GUI::UICanvas::RemoveElement(UIElementID id)
    {
        std::erase_if(m_rootElements, [id](const Ref<GUIElement>& child) { return child->GetID() == id; });
        m_needsSort = true;
    }

    void UICanvas::SetSize(Vector2 size) 
    {
        m_size = size;
        for (auto& root : m_rootElements) 
            root->InvalidateTransform(); 
    }

    void UICanvas::BringToFront(UIElementID id) 
    {
        auto it = std::find_if(m_rootElements.begin(), m_rootElements.end(), 
            [id](const auto& e) { return e->GetID() == id; });

        if (it == m_rootElements.end()) return;

        int maxZ = 0;
        for (const auto& root : m_rootElements)
            if (root->m_zIndex > maxZ) maxZ = root->m_zIndex;

        (*it)->m_zIndex = maxZ + 1;
        m_needsSort = true;
        SortRootElements();
    }

    void pgn::GUI::UICanvas::SortRootElements()
    {
        if (m_needsSort) 
        {
            std::stable_sort(m_rootElements.begin(), m_rootElements.end(), 
                [](const auto& a, const auto& b) { return a->GetZIndex() < b->GetZIndex(); });
            m_needsSort = false;
        }
    }

    void UICanvas::CleanupDeadElements() 
    {
        std::erase_if(m_rootElements, [](auto& e) {return e->m_isDead; });
    }

    void UICanvas::OnUpdate(Timestep dt) 
    {
        if (!m_isEnabled) return;
        SortRootElements();
        for (auto& root : m_rootElements) root->OnUpdate(dt);
    }

    void UICanvas::OnRender(float alpha) 
    {
        if (!m_isEnabled) return;
        for (auto& root : m_rootElements) root->OnRender(alpha);
    }

    bool UICanvas::OnEvent(Event& e) 
    {
        if (!m_isEnabled) return false;

        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& ev) {
            SetSize({ (float)ev.GetWidth(), (float)ev.GetHeight() });
            return false; 
        });

        // Focus logic
        dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& ev) {
            for (auto it = m_rootElements.rbegin(); it != m_rootElements.rend(); ++it) 
            {
                if ((*it)->GetGlobalBounds().contains(ev.GetPosition())) 
                {
                    BringToFront((*it)->GetID());
                    break; 
                }
            }
            return false; 
        });

        bool handled = false;
        for (auto it = m_rootElements.rbegin(); it != m_rootElements.rend(); ++it) 
        {
            if ((*it)->OnEvent(e)) 
            {
                handled = true;
                break;
            }
        }

        if (!handled && m_isEventBlocking) 
        {
            if (e.IsInCategory(EventCategoryMouse) || e.IsInCategory(EventCategoryKeyboard))
                return true; 
        }

        return handled;
    }
}