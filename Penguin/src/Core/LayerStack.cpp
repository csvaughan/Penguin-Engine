#include "Core/LayerStack.h"
#include "Log/Log.h"
#include "Core/Layer.h"
#include "Core/Input.h"
#include "Events/EngineEvents.h"
#include "Events/InputEvents.h"

namespace pgn
{
    LayerStack::~LayerStack() 
    {
        for (auto& layer : m_layers)
            layer->OnExit();
    }

    void LayerStack::pushLayer(Ref<Layer> layer) { m_pendingActions.push_back({ PendingAction::Push, std::move(layer) }); }

    void LayerStack::pushOverlay(Ref<Layer> overlay) { m_pendingActions.push_back({ PendingAction::PushOverlay, std::move(overlay) }); }

    void LayerStack::popLayer(Ref<Layer> layer) { m_pendingActions.push_back({ PendingAction::Pop, std::move(layer) }); }

    void LayerStack::applyPendingChanges() 
    {
        for (auto& action : m_pendingActions) 
        {
            switch (action.Type) 
            {
                case PendingAction::Push: 
                {
                    action.layer->OnEnter();
                    LayerPushEvent e(action.layer);
                    EventBus::Publish(e);
                    m_layers.emplace(m_layers.begin() + m_layerInsertIndex, action.layer);
                    m_layerInsertIndex++;
                    break;
                }
                case PendingAction::PushOverlay:
                {
                    action.layer->OnEnter();
                    LayerPushEvent e(action.layer);
                    EventBus::Publish(e);
                    m_layers.emplace_back(action.layer);
                    break;
                }
                case PendingAction::Pop: 
                {
                    auto it = std::find(m_layers.begin(), m_layers.end(), action.layer);
                    if (it != m_layers.end()) 
                    {
                        action.layer->OnExit();
                        LayerPopEvent e(action.layer);
                        EventBus::Publish(e);
            
                        if (std::distance(m_layers.begin(), it) < m_layerInsertIndex) 
                            m_layerInsertIndex--;

                        m_layers.erase(it);
                    }
                    break;
                }
            }
        }
        m_pendingActions.clear();
    }

    void LayerStack::update(Timestep ts)
    {
        for (const auto& layer : m_layers)
            layer->OnUpdate(ts);
    }

    void LayerStack::handleEvent(Event& event) 
    {
        for (auto it = m_layers.rbegin(); it != m_layers.rend(); ++it) 
        {
            if (event.Handled) break; 
            (*it)->OnEvent(event); 
        }
    }

    void LayerStack::render(float alpha)
    {
        for (const auto& layer : m_layers)
            layer->OnRender(alpha);         
    }   

    void LayerStack::broadcastShutdown(AppQuitEvent& e)
    {
        for (auto& layer : m_layers)
            layer->OnAppQuit(e);
    }
}