#pragma once
#include "Penguin.h"

class Layer1 : public pgn::Layer
{
public:
    Layer1(/* args */) : pgn::Layer("Layer1") {}
    ~Layer1() = default;


    void OnUpdate(pgn::Timestep ts) override
    {

    }

private:
    /* data */
};
