#pragma once
#include "Penguin.h"

class GameUILayer : public pgn::Layer
{
    public:
        GameUILayer(const std::string name) : pgn::Layer(name) {}
        ~GameUILayer(){}

        void OnEnter() override 
        {
            pgn::Vector2 ss = App().GetWindow().GetWindowSize();
            auto canvas = m_uiManager.CreateCanvas();

            auto img = canvas->CreateElement<pgn::GUI::ImageBox>(pgn::Vector2{-100, 300}, pgn::Vector2{96, 80});
            img->SetImage(App().GetTexture("coin"));

            auto anim = canvas->CreateElement<pgn::GUI::AnimatedImageBox>(pgn::Vector2{100, 300}, pgn::Vector2{300, 50});
            anim->SetAnimation(pgn::Animation::CreateFromSheet(App().GetTexture("coin_flip"), 96, 144, 0.2f, pgn::LoopMode::Single));

            auto btn = canvas->CreateElement<pgn::GUI::Button>(pgn::Vector2{0, 0}, pgn::Vector2{300, 70});
            btn->SetBaseColor(pgn::Color::Grey);
            btn->SetCallback([this, img]() { FlipCoin(img); });
        }

        void OnEvent(pgn::Event& e) { m_uiManager.OnEvent(e); }

        void OnUpdate(pgn::Timestep ts) 
        {
            m_uiManager.OnUpdate(ts); 
        }

        void OnRender(float alpha){ m_uiManager.OnRender(alpha); }

        void FlipCoin(pgn::GUI::ImageBox* img)
        {
        }

    private:

        pgn::GUI::UIManager m_uiManager;  
};