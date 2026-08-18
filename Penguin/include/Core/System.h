#pragma once
#include "pgpch.h"
#include "Events/EventBus.h"
#include "Events/EventToken.h"
#include "Core/Timestep.h"
#include "Memory/Scope.h"

namespace pgn {

    class AppQuitEvent;
    class Renderer;

    class BaseSystem 
    {
    public:
        BaseSystem (const std::string& name) : m_name(name), m_app(Application::Get()), m_assets(m_app.GetAssetManager()), m_window(*m_app.Get().GetWindow()) {}
        virtual ~BaseSystem () {m_tokens.clear(); } 

        const std::string& GetName() {return m_name; }

    protected:

        //Application getters
        Vector2 GetFramebufferSize() const          { return m_app.GetFramebufferSize(); }
        Window& GetWindow() const                   { return m_window; }
        double GetTime()                            { return m_app.GetTime(); }
        float GetFPS()                              { return m_app.GetFPS(); }
        AssetManager& Assets()                      { return m_assets; }

        //Events Might add back in later
        void RaiseEvent(Event& event) { m_app.RaiseEvent(event); }

        //Game Loop
        virtual void OnEnter() {}
        virtual void OnExit() {}
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnRender(float alpha, Renderer& renderer) {} 
        
        virtual void OnEvent(Event& e) 
        {
            if (e.Handled) return;
            auto it = m_handlers.find(e.GetEventType());
            if (it != m_handlers.end()) 
                e.Handled = it->second(e);
        }

        virtual void OnAppQuit(AppQuitEvent& e){}


         /**
         * Global Events (broadcasts using the Event Bus).
         */
        template<typename TEvent, typename TObject>
        requires(std::is_base_of_v<Event, TEvent>)
        void Subscribe(void (TObject::*func)(TEvent&)) 
        {
            size_t id = EventBus::Subscribe<TEvent>([this, func](Event& e) { (static_cast<TObject*>(this)->*func)(static_cast<TEvent&>(e)); });
            m_tokens.push_back(std::make_unique<EventToken>(TEvent::GetStaticType(), id));
        }

        template<typename TEvent>
        requires(std::is_base_of_v<Event, TEvent>)
        void Subscribe(std::function<void(TEvent&)> func) 
        {
            size_t id = EventBus::Subscribe<TEvent>([func](Event& e) { func(static_cast<TEvent&>(e)); });
            m_tokens.push_back(std::make_unique<EventToken>(TEvent::GetStaticType(), id));
        }

        /**
         * Local Event handling.
         */
        template<typename TEvent, typename TObject>
        requires(std::is_base_of_v<Event, TEvent>)
        void BindEvent(TObject* instance, bool (TObject::*func)(TEvent&)) 
        {
            m_handlers[TEvent::GetStaticType()] = [instance, func](Event& e) { return (instance->*func)(static_cast<TEvent&>(e)); };
        }

    private:

        std::string m_name;
        std::unordered_map<EventType, std::function<bool(Event&)>> m_handlers;
        std::vector<Scope<EventToken>> m_tokens;

        Application& m_app;
        AssetManager& m_assets;
        Window& m_window;
    };
}