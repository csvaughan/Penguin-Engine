#pragma once
#include "LayerStack.h"
#include "System.h"
#include "Layer.h"

namespace pgn
{
    class LayerPushEvent;
    class LayerPopEvent;

    class State : private ISystem
    {
    public:
        State(const std::string& name);
        virtual ~State() = default;
    
    protected:
        template<typename TState, typename... Args>
        requires(std::is_base_of_v<State, TState>)
        void ChangeState(Args&&... args)
        {
            changeState(std::make_unique<State>(std::forward<Args>(args)...));
        }

        template<typename TLayer, typename... Args>
        requires(std::is_base_of_v<Layer, TLayer>)
        Ref<TLayer> PushLayer(const std::string& name, Args&&... args)
        {
            if(!HasLayer(name))
            {
                auto l = createLayerPtr<TLayer>(name, std::forward<Args>(args)...);
                m_layers.pushLayer(std::move(l));
                return l;
            }
            else
            {
                PGN_WARN("Warning: Layer creation failed. Layer: {} already exists.", name);
                return StaticRefCast<TLayer>(getLayer(name));
            }
        }
        
        template<typename TLayer, typename... Args>
        requires(std::is_base_of_v<Layer, TLayer>)
        Ref<TLayer> PushOverlay(const std::string& name, Args&&... args)
        {
            if(!HasLayer(name))
            {
                auto l = createLayerPtr<TLayer>(name, std::forward<Args>(args)...);
                m_layers.pushOverlay(std::move(l));
                return l;
            }
            else
            {
                PGN_WARN("Warning: Overlay creation failed. Overlay: {} already exists.", name);
                return StaticRefCast<TLayer>(getLayer(name));
            }
        }

        void PopLayer(const std::string& name);
        bool HasLayer(const std::string& name);

        virtual void OnLayerPop(LayerPopEvent& e);
        virtual void OnLayerPush(LayerPushEvent& e);

        virtual void OnEnter() override {}
        virtual void OnExit() override {}

        using ISystem::GetName;
        using ISystem::App;
        using ISystem::OnAppQuit;
        using ISystem::BindEvent;
        using ISystem::Subscribe;
        
    private:
        
        void OnEvent(Event& e) final override;
        void OnUpdate(Timestep dt) final override;
        void OnRender(float alpha) final override;

        //Private helpers
        Ref<Layer> getLayer(const std::string& name);
        void changeState(Scope<State> newState); 
        auto findLayer(const std::string& name) { return m_layerRefMap.find(name);}
        
        template<typename TLayer, typename... Args>
        Ref<TLayer> createLayerPtr(const std::string& name, Args&&... args)
        {
            auto layer = CreateRef<TLayer>(name, std::forward<Args>(args)...);
            m_layerRefMap[name] = layer; 
            layer->m_parentState = this;
            return layer;
        }

    private:
        LayerStack m_layers;
        std::map<std::string, Ref<Layer>> m_layerRefMap;

        friend class Layer;
        friend class Application;
    };
}