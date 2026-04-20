#pragma once
#include "Penguin.h"
#include "Layer1.h"

class MenuState : public pgn::State
{
public:
    MenuState() : pgn::State("MenuState"){}

    void OnEnter() override
    {
        PGN_INFO("MenuState created.");
        PushLayer<Layer1>();
        //pgn::Input::LoadConfig("Input.cfg");

        
    }

    void OnExit() override
    {
        //pgn::Input::clearConfig();
    }
};
