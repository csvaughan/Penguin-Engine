#pragma once
#include "Event.h"
#include "Core/State.h"
#include "Core/Layer.h"
#include "Memory/Scope.h"

namespace pgn
{
    class StateChangeEvent : public Event
    {
    public:
        StateChangeEvent(Scope<State> newState) : m_NewState(std::move(newState)) {}

        Scope<State> GetState() { return std::move(m_NewState); }

        EVENT_CLASS_TYPE(StateChange)
        EVENT_CLASS_CATEGORY(EventCategoryEngine)
    private:
        Scope<State> m_NewState;
    };

    class AppQuitEvent : public Event
    {
    public:
        enum class QuitReason { UserExit, Error, Restart };

        AppQuitEvent(QuitReason reason = QuitReason::UserExit) : m_Reason(reason) {}
        
        QuitReason GetReason() const { return m_Reason; }
        
        EVENT_CLASS_TYPE(AppQuit)
        EVENT_CLASS_CATEGORY(EventCategoryEngine)
    private:
        QuitReason m_Reason;
    };

    class LayerPushEvent : public Event
    {
    public:
        LayerPushEvent(Ref<Layer> layer) : m_layer(layer){}
        Ref<Layer> GetLayer() { return m_layer; }
        EVENT_CLASS_TYPE(LayerPush);
        EVENT_CLASS_CATEGORY(EventCategoryEngine)
    private:
        Ref<Layer> m_layer;
    };

    class LayerPopEvent : public Event
    {
    public:
        LayerPopEvent(Ref<Layer> layer) : m_layer(layer){}
        Ref<Layer> GetLayer() { return m_layer; }
        EVENT_CLASS_TYPE(LayerPop);
        EVENT_CLASS_CATEGORY(EventCategoryEngine)
    private:
        Ref<Layer> m_layer;
    };

} // namespace pgn