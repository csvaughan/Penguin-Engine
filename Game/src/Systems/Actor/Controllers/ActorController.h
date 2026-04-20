#pragma once
#include "Game/Commands/ActorCommands.h"
#include "Engine.h"

enum class ControllerType {Player, AI};

class ActorController
{
public:
    ActorController(Engine::ECS::Entity target, ControllerType type) : m_target(target), m_active(true) { }
    ~ActorController() {}

    void stop() {}
    bool isActive() {return m_active;}
    void setActive(bool active) {m_active = active;}
    virtual void update() = 0;

protected:
    ControllerType type;
    bool m_active;
    Engine::ECS::Entity m_target;
};