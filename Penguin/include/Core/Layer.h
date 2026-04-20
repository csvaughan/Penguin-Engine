#pragma once
#include "System.h"
#include "Memory/Ref.h"

namespace pgn
{
    class State;

    class Layer : private ISystem, public RefCounted
    {
    public:
        Layer(const std::string& name, bool enabled = false);
        virtual ~Layer();

        const bool IsEnabled() const { return m_isEnabled;}
        void setEnabled(bool tf) { m_isEnabled = tf;}

    protected:

        virtual void OnUpdate(Timestep) override {} 
        virtual void OnRender(float alpha) override {}
        virtual void OnEnter() override{}
        virtual void OnExit() override {}

        using ISystem::GetName;
        using ISystem::App;
        using ISystem::OnAppQuit;
        using ISystem::BindEvent;
        using ISystem::Subscribe;

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