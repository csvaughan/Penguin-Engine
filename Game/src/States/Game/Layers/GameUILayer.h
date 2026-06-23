#pragma once
#include "Penguin.h"

class GameUILayer : public pgn::Layer
{
    public:
        GameUILayer(const std::string name) : pgn::Layer(name) {}
        ~GameUILayer(){}

        void OnEnter() override 
        {
            auto canvas = m_uiManager.CreateCanvas(App().GetWindow().GetWindowSize());

            auto btn = canvas->CreateElement<pgn::GUI::Button>(pgn::Vector2{0, 0}, pgn::Vector2{280, 80});
            btn->SetTexture(App().GetTexture("button"));
            btn->SetText("Start Game", App().GetFont("default_font"));
            btn->SetCallback([this, btn]() { FlipCoin(btn); });
            
            auto volumeSlider = canvas->CreateElement<pgn::GUI::Slider>(pgn::Vector2{150, 300}, pgn::Vector2{20, 300}, pgn::GUI::UILayoutDirection::Vertical,"VolSlider");

            // 2. Setup colors and bounding rules
            volumeSlider->SetRange(0.0f, 100.0f);
            volumeSlider->SetFillColor(pgn::Color::Red);
            volumeSlider->SetHandleColor(pgn::Color::Grey);
            volumeSlider->SetStepSize(10);
            //volumeSlider->SetValue(75.0f);

            // 3. Connect a dynamic structural execution block
            volumeSlider->SetCallback([](float value) { std::println("Volume adjusted to: {}", value); });

        }

        void OnEvent(pgn::Event& e) { m_uiManager.OnEvent(e); }

        void OnUpdate(pgn::Timestep ts) 
        {
            m_uiManager.OnUpdate(ts); 
        }

        void OnRender(float alpha){ m_uiManager.OnRender(alpha); }

        void FlipCoin(pgn::GUI::Button* btn)
        {
            std::println("Hello from {}", btn->GetName());
        }

    private:

        pgn::GUI::UIManager m_uiManager;  
};