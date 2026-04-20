#pragma once
#include "Math/Transform.h"
#include "Math/Rect.h"
#include "Core/Timestep.h"
#include "Core/AppContext.h"
#include "Memory/Ref.h"

namespace pgn::GUI {

    class UICanvas;
    using UIElementID = size_t;

    enum class UIState  { Normal, Hovered, Pressed, Disabled };
    enum class Anchor   { TopLeft, TopCenter, TopRight, CenterLeft, Center, CenterRight, BottomLeft, BottomCenter, BottomRight };
    enum class Pivot    { TopLeft, Center, BottomRight };

    class GUIElement : public RefCounted
    {
        friend class UICanvas; 

    public:
        GUIElement(UICanvas* canvas, Vector2 pos, Vector2 size, const std::string& name, GUIElement* parent = nullptr);
        virtual ~GUIElement();

        template<typename TElement, typename... Args>
        requires(std::is_base_of_v<GUIElement, TElement>)
        TElement* AddChild(Args&&... args) 
        {
            auto e = CreateRef<TElement>(m_canvas, std::forward<Args>(args)..., this);
            m_children.push_back(e);
            m_needsSort = true;
            return e.get(); 
        }

        void RemoveChild(UIElementID id);

        void Destroy() { m_isDead = true; }

        // --- Transform & Bounds ---
        Vector2 GetScreenPos() const; 
        FloatRect GetGlobalBounds() const;
        Vector2 GetLocalSize() const { return { m_bounds.w, m_bounds.h };} 
        virtual Vector2 GetInternalOffset() const { return { 0.0f, 0.0f }; };
        
        void SetPosition(Vector2 pos) { m_transform.position = pos; InvalidateTransform(); }
        void SetSize(Vector2 size) { m_bounds.w = size.x; m_bounds.h = size.y; InvalidateTransform(); }
        void SetAnchor(Anchor a) { m_anchor = a; InvalidateTransform(); }
        void SetPivot(Pivot p) { m_pivot = p; InvalidateTransform(); }
        void SetZIndex(int z);

        void SetVisible(bool visible) 
        { 
            m_visible = visible;
            SetEnabled(visible); 
            for (auto c : m_children) 
                c->SetVisible(visible);
        }

        void SetEnabled(bool enabled) 
        { 
            m_enabled = enabled; 
            for (auto c : m_children) 
                c->SetEnabled(enabled);
        }

        UIElementID GetID() const { return m_id; }
        int GetZIndex() const { return m_zIndex; }
        bool IsVisible() const { return m_visible; }
        bool IsEnabled() const { return m_enabled; }

        void Render(float alpha) 
        { 
            if (!m_visible) return; 
            OnRender(alpha);       
            RenderChildren(alpha); 
        }

    protected:
        virtual void OnUpdate(Timestep dt);
        virtual void OnRender(float alpha) {}
        virtual bool OnEvent(Event& e);

        bool Contains(Vector2 point) {return GetGlobalBounds().contains(point); }
        void SortChildren();
        void InvalidateTransform();
        AppContext& GetContext();

        virtual Vector2 ScreenToLocal(Vector2 screenPoint) const;

        virtual void RenderChildren(float alpha) 
        {
            for (auto& child : m_children) 
                child->Render(alpha);
        }

    protected:
        static std::atomic<UIElementID> s_idCounter;
        UIElementID m_id;
        UICanvas* m_canvas;
        GUIElement* m_parent;
        std::string m_name;
        
        Transform2D m_transform;
        FloatRect m_bounds; // Reference-space bounds
        int m_zIndex = 0;
        
        mutable bool m_dirty = true;
        mutable Vector2 m_cachedScreenPos{0.0f, 0.0f};

        Anchor m_anchor = Anchor::Center;
        Pivot  m_pivot =   Pivot::Center;

        std::vector<Ref<GUIElement>> m_children;
        bool m_enabled = true;
        bool m_visible = true;
        bool m_needsSort = false;
        bool m_isDead = false;
    };
}