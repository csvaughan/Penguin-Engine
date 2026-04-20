#pragma once
#include "Renderer/Renderer.h"
#include "UICanvas.h"

namespace pgn::GUI
{
    class UIManager
    {
    public:
        UIManager(){}

        UICanvas* CreateCanvas()
        {
            auto c = CreateScope<UICanvas>();
            UICanvas* ptr = c.get(); 
            m_canvases.emplace_back(std::move(c));
            return ptr; 
        }

        void LoadUILayout(std::string file)
        {

        }

        void OnEvent(Event& e)
        { 
            for (auto it = m_canvases.rbegin(); it != m_canvases.rend(); ++it)
            {
                auto& c = *it;
                if (c->IsEnabled())
                {
                    if (c->OnEvent(e))
                        break; 
                }
            }
        }

        void OnUpdate(Timestep ts)
        { 
            for (auto& c : m_canvases) 
            {
                if(c->IsEnabled())
                    c->OnUpdate(ts);
            }
        }

        void OnRender(float alpha)
        {
            Renderer::BeginScene(); 

            for (auto& c : m_canvases)
            {
                if (c->IsEnabled())
                    c->OnRender(alpha);
            }

            Renderer::EndScene();
        }

    private:
        std::vector<Scope<UICanvas>> m_canvases;
    };
}