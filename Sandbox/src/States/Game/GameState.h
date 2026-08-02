#pragma once
#include <Penguin.h>

class GameState : public pgn::State
{
public:
    GameState () : pgn::State("GameState"){}

    void OnEnter() override;
    void OnExit() override;

    //void OnLayerPush(pgn::LayerPushEvent& e) override {}
    bool SwitchLayer(pgn::KeyPressedEvent& e);
};