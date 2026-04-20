#pragma once
#include "Game/Commands/ActorCommands.h"
#include "Game/Actor/Controllers/ActorController.h"

class PlayerController : public ActorController
{
public:
    PlayerController(Engine::ECS::Entity& target) : ActorController(target, ControllerType::Player){}
    ~PlayerController() {}

    void update() override
    {
        if (m_active)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    {ActorCommand::Move(m_target, Direction::Up).execute(); return;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  {ActorCommand::Move(m_target, Direction::Down).execute(); return;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  {ActorCommand::Move(m_target, Direction::Left).execute(); return;}
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {ActorCommand::Move(m_target, Direction::Right).execute(); return;}
            else {ActorCommand::Stop(m_target).execute(); return;}
        }
    }
};

struct ActorControllerComponent : public Engine::ECS::Component
{
    std::shared_ptr<ActorController> controller;
};