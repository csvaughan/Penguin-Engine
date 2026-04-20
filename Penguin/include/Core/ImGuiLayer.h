#pragma once
#include "Core/Layer.h"

#ifdef PGN_DEBUG
#include <imgui.h>
#endif

namespace pgn
{
    class ImguiLayer : public Layer
    {
    public:
        ImguiLayer(const std::string& name) : Layer(name) {}
        
        virtual ~ImguiLayer() {}

        virtual void OnImGuiEvent(Event& e) {}
        virtual void OnImGuiUpdate(Timestep ts) {}
        virtual void OnImGuiRender() = 0;

    private:

        void OnEvent(Event& e) final override
        {
            #ifdef PGN_DEBUG
            OnImGuiEvent(e);
            #endif
        }
        
        void OnUpdate(Timestep ts) final override
        {
            #ifdef PGN_DEBUG
            OnImGuiUpdate(ts);
            #endif
        }
        
        void OnRender(float alpha) final override
        {
            #ifdef PGN_DEBUG
            OnImGuiRender();
            #endif
        }
    };
}