#pragma once
#include "AppContext.h"

namespace pgn {

    class AppQuitEvent;

    class ISystem
    {
    public:
        ISystem(const std::string& name) : m_name(name) {}
        virtual ~ISystem() {m_tokens.clear(); } 

    protected:

        const std::string& GetName() {return m_name; }

        virtual void OnEnter() {}
        virtual void OnExit() {}
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnRender(float alpha) {} 
        
        virtual void OnEvent(Event& e) 
        {
            if (e.Handled) return;
            auto it = m_handlers.find(e.GetEventType());
            if (it != m_handlers.end()) 
                e.Handled = it->second(e);
        }

        virtual void OnAppQuit(AppQuitEvent& e){}

        AppContext& App() {return m_context; }

         /**
         * Use this for Global Events (broadcasts using the Event Bus).
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
         * Use this for Local Event handling.
         */
        template<typename TEvent, typename TObject>
        requires(std::is_base_of_v<Event, TEvent>)
        void BindEvent(TObject* instance, bool (TObject::*func)(TEvent&)) 
        {
            m_handlers[TEvent::GetStaticType()] = [instance, func](Event& e) { return (instance->*func)(static_cast<TEvent&>(e)); };
        }

    private:
        AppContext m_context;
        std::string m_name;
        std::unordered_map<EventType, std::function<bool(Event&)>> m_handlers;
        std::vector<Scope<EventToken>> m_tokens;
    };

    // Used as a base for client-side systems
    class BaseSystem : private ISystem 
    {
    public:
        BaseSystem(const std::string& name) : ISystem(name) {}
        virtual ~BaseSystem() = default;

        using ISystem::GetName;
        
    protected:

        using ISystem::App;
        using ISystem::OnUpdate;
        using ISystem::OnRender;
        using ISystem::OnEvent;
        using ISystem::OnEnter;
        using ISystem::OnExit;
        using ISystem::OnAppQuit;
        using ISystem::BindEvent;
        using ISystem::Subscribe;
    };
}