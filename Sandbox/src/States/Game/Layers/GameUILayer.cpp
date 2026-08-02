#include "GameUILayer.h"

GameUILayer::GameUILayer(const std::string name) : pgn::Layer(name) {}

GameUILayer::~GameUILayer() {}

void GameUILayer::OnEnter()
{
    auto canvas = m_uiManager.CreateCanvas(App().GetWindow().GetWindowSize());

    auto btn = canvas->CreateElement<pgn::GUI::Button>(pgn::Vector2{0, 0}, pgn::Vector2{280, 80});
    btn->SetTexture(App().GetTexture("button"));
    btn->SetText("Start Game", App().GetFont("default_font"));
    
    auto volumeSlider = canvas->CreateElement<pgn::GUI::Slider>(pgn::Vector2{150, 300}, pgn::Vector2{20, 300}, pgn::GUI::UILayoutDirection::Vertical,"VolSlider");
    volumeSlider->SetRange(0.0f, 100.0f);
    volumeSlider->SetFillColor(pgn::Color::Red);
    volumeSlider->SetHandleColor(pgn::Color::Grey);
    volumeSlider->SetStepSize(10);
    volumeSlider->SetValue(75.0f);

    volumeSlider->SetCallback([](float value) { std::println("Volume adjusted to: {}", value); });
}

void GameUILayer::OnEvent(pgn::Event& e)
{ 
    m_uiManager.OnEvent(e);
}

void GameUILayer::OnUpdate(pgn::Timestep ts)
{
    m_uiManager.OnUpdate(ts); 
}

void GameUILayer::OnRender(float alpha)
{ 
    m_uiManager.OnRender(alpha);
}