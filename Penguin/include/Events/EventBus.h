#pragma once
#include "Event.h"
#include <mutex>      
#include <shared_mutex>

namespace pgn {

    class EventBus 
    {
    public:
        using EventCallback = std::function<void(Event&)>;

        template<typename T>
        static size_t Subscribe(EventCallback&& callback) 
        {
            auto& bus = Get();
            std::unique_lock lock(bus.m_QueueMutex);
            
            size_t id = bus.m_NextID++;
            bus.m_PendingActions.push_back({ ActionType::Subscribe, T::GetStaticType(), id, std::move(callback) });
            return id;
        }

        static void Unsubscribe(EventType type, size_t id) 
        {
            auto& bus = Get();
            std::unique_lock lock(bus.m_QueueMutex);
            bus.m_PendingActions.push_back({ ActionType::Unsubscribe, type, id, nullptr });
        }

        static void Publish(Event& e) 
        {
            auto& bus = Get();
        
            std::shared_ptr<const ListenerMap> currentListeners;
            {
                std::shared_lock lock(bus.m_BusMutex);
                currentListeners = bus.m_ActiveListeners;
            }

            if (!currentListeners) return;

            auto it = currentListeners->find(e.GetEventType());
            if (it != currentListeners->end()) 
            {
                // We are now iterating over a point-in-time snapshot.
                // Even if another thread changes the "Active" map, this pointer stays valid.
                for (auto const& [id, callback] : it->second) 
                {
                    if (e.Handled) break;
                    callback(e);
                }
            }
        }

        private:

            static void ProcessPending() 
            {
                auto& bus = Get();
                
                std::vector<PendingAction> actions;
                {
                    std::unique_lock lock(bus.m_QueueMutex);
                    if (bus.m_PendingActions.empty()) return;
                    actions.swap(bus.m_PendingActions);
                }

                auto newListeners = std::make_shared<ListenerMap>(*bus.m_ActiveListeners);

                for (auto& action : actions) 
                {
                    if (action.Type == ActionType::Subscribe) 
                        (*newListeners)[action.EType][action.ID] = std::move(action.Callback);
                    else 
                        (*newListeners)[action.EType].erase(action.ID);
                }

                {
                    std::unique_lock lock(bus.m_BusMutex);
                    bus.m_ActiveListeners = std::move(newListeners);
                }
            }

        static EventBus& Get() { static EventBus instance; return instance; }

        enum class ActionType { Subscribe, Unsubscribe };
        struct PendingAction 
        {
            ActionType Type;
            EventType EType;
            size_t ID;
            EventCallback Callback;
        };
        
        std::mutex m_QueueMutex; // Protects the pending actions vector
        using ListenerMap = std::unordered_map<EventType, std::unordered_map<size_t, EventCallback>>;
            
        // Use an atomic-wrapped shared_ptr for the fastest possible swap
        std::shared_ptr<ListenerMap> m_ActiveListeners = std::make_shared<ListenerMap>();
        std::shared_mutex m_BusMutex; 
        
        size_t m_NextID = 0;
        std::vector<PendingAction> m_PendingActions;

        friend class Application;
    };
}