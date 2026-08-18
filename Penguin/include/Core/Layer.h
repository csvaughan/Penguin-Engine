#pragma once
#include "System.h"
#include "Memory/Ref.h"

namespace pgn
{
    class State;
    class Renderer;

    class Layer : private BaseSystem, public RefCounted
    {
    public:
        Layer(const std::string& name, bool enabled = false);
        virtual ~Layer();

        const bool IsEnabled() const { return m_isEnabled;}
        void setEnabled(bool tf) { m_isEnabled = tf;}

    protected:

        virtual void OnUpdate(Timestep) override {} 
        virtual void OnRender(float alpha, Renderer& renderer) override {}
        virtual void OnEnter() override {}
        virtual void OnExit() override {}

        using BaseSystem::GetName;
        using BaseSystem::OnAppQuit;
        using BaseSystem::BindEvent;
        using BaseSystem::Subscribe;

        using BaseSystem::GetFramebufferSize;    
        using BaseSystem::GetWindow;            
        using BaseSystem::GetTime;                          
        using BaseSystem::GetFPS;                          
        using BaseSystem::Assets;
        using BaseSystem::RaiseEvent;

        template<typename TState, typename... Args>
        requires(std::is_base_of_v<State, TState>)
        void RequestStateChange(Args&&... args) { InternalRequestStateChange(std::make_unique<TState>(std::forward<Args>(args)...)); }

    private:
        void InternalRequestStateChange(Scope<State> newState);
        State* m_parentState;

        // If true: events pass through this layer to the ones below even if this layer has handlers. 
        // If false: once a handler returns 'true', the event stops here.
        bool m_isEnabled = false;

        friend class State;
        friend class LayerStack;
    };
}