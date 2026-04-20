#include "Core/Layer.h"
#include "Core/State.h"

namespace pgn
{
    Layer::Layer(const std::string &name, bool transparent)
    : ISystem(name), m_parentState(nullptr), m_isEnabled(transparent)
    {
        PGN_CORE_INFO("Layer {} created.(Layer Transparentcy set to {}.)", name, (m_isEnabled) ? "True" : "False");
    }

    Layer::~Layer() = default;

    void Layer::InternalRequestStateChange(Scope<State> newState)
    {
        if (m_parentState) {m_parentState->changeState(std::move(newState));} 
    }
}   