#include "GameState.h"
#include "Layers/GameLayer.h"
#include "Layers/GameUILayer.h"

void GameState::OnEnter()
{
    PGN_INFO("GameState created.");
    App().LoadTexture("WD", "textures/warrior/Down/WarriorDownIdle.png");
    App().LoadSound("test","audio/lazer-gun.mp3");
    App().LoadTexture("coin", "textures/coin/coin_heads.png");
    App().LoadTexture("coin_flip", "textures/coin/coinflip.png");
    App().LoadTexture("button", "textures/GUI/png/Button/Rect/Default.png");

    BindEvent<pgn::KeyPressedEvent>(this, &GameState::SwitchLayer);
    //pgn::Input::LoadConfig("Input.cfg");
    
    PushOverlay<GameUILayer>("TestUILayer");
    PushLayer<GameLayer>("GameLayer"); 
}

void GameState::OnExit()
{
    //pgn::Input::clearConfig();
}

//void OnLayerPush(pgn::LayerPushEvent& e) override {}

bool GameState::SwitchLayer(pgn::KeyPressedEvent& e)
{
    if(e.GetKeyCode() == pgn::KeyCode::S)
        if(!HasLayer("DebugLayer"))
            PushLayer<pgn::DebugLayer>("DebugLayer");
        else
            PopLayer("DebugLayer");
    
    return true;
}