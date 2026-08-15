#pragma once
#include "Penguin.h"

class GameUILayer : public pgn::Layer
{
public:
    GameUILayer(const std::string name);
    virtual ~GameUILayer() override;

    void OnEnter() override;
    void OnEvent(pgn::Event& e) override;
    void OnUpdate(pgn::Timestep ts) override;
    void OnRender(float alpha, pgn::Renderer& renderer) override;

private:
    pgn::GUI::UIManager m_uiManager; 
};