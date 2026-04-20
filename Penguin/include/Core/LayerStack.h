#pragma once
#include "pgpch.h"
#include "Timestep.h"
#include "Memory/Ref.h"

namespace pgn
{
    class Layer;
    class Event;
    class AppQuitEvent;

    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack();

        void pushLayer(Ref<Layer> layer);
        void pushOverlay(Ref<Layer> overlay);
        void popLayer(Ref<Layer> layer); 

        void applyPendingChanges();

        void update(Timestep ts);
        void render(float alpha);
        void handleEvent(Event& event);

        void broadcastShutdown(AppQuitEvent& e);

        std::vector<Ref<Layer>>::iterator begin() { return m_layers.begin(); }
        std::vector<Ref<Layer>>::iterator end() { return m_layers.end(); }

    private:

        struct PendingAction 
        {
            enum { Push, PushOverlay, Pop } Type;
            Ref<Layer> layer;
        };

        std::vector<Ref<Layer>> m_layers;
        std::vector<PendingAction> m_pendingActions;
        unsigned int m_layerInsertIndex = 0;
    };
}