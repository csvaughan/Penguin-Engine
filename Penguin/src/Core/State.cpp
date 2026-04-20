#include "Core/Application.h"
#include "Core/State.h"
#include "Core/Layer.h"
#include "Events/EngineEvents.h"

namespace pgn
{
    State::State(const std::string& name) : ISystem(name) 
    {
        Subscribe<LayerPushEvent>([this](LayerPushEvent& e) {this->OnLayerPush(e);}); 
        Subscribe<LayerPopEvent>([this](LayerPopEvent& e) {this->OnLayerPop(e);});
    }

    void State::changeState(Scope<State> newState)
    {
        StateChangeEvent e(std::move(newState));
        Application::Get().RaiseEvent(e);
    }

    void State::OnEvent(Event& e) 
    {
        m_layers.handleEvent(e); 
        if (!e.Handled) 
            ISystem::OnEvent(e);
    }

    void State::OnUpdate(Timestep dt) 
    {
        m_layers.applyPendingChanges();
        m_layers.update(dt);  
    }

    void State::OnRender(float alpha) 
    { 
        m_layers.render(alpha);
    }

    void State::OnLayerPop(LayerPopEvent& e){PGN_CORE_INFO("Layer: {} popped from stack.", e.GetLayer()->GetName());}

    void State::OnLayerPush(LayerPushEvent& e){PGN_CORE_INFO("Layer: {} pushed to stack.", e.GetLayer()->GetName());}

    bool State::HasLayer(const std::string& name) { return getLayer(name).get() != nullptr; }

    void State::PopLayer(const std::string &name)
    {
        auto layer = getLayer(name); 
        if (layer)
        {
            m_layers.popLayer(layer.get());
            m_layerRefMap.erase(name);
        }
        else
        {
            PGN_CORE_WARN("Attempted to pop layer '{0}', but it wasn't found or was already destroyed.", name);
        }
    }

    Ref<Layer> State::getLayer(const std::string &name)
    {
        auto it = m_layerRefMap.find(name);
        return (it != m_layerRefMap.end()) ? it->second : nullptr;
    }
} // namespace pgn