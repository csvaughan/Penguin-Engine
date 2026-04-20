#pragma once
#include "Controllers/PlayerController.h"
#include "Controllers/AIController.h"
#include "Engine.h"

class DataManager;

class ActorManager
{
public:
    using EntityID  = size_t;

    ActorManager(Engine::ECS::Registry&, DataManager&);
    ~ActorManager() = default;

    Engine::ECS::Entity createActor(const std::string&, ActorType, const sf::Vector2f);

    void addController(Engine::ECS::Entity&, ControllerType);
    void removeController(Engine::ECS::Entity&);
    bool hasController(Engine::ECS::Entity&) const;

    void updateActors(float);
    void renderActors();

private:

    Engine::ECS::Registry& m_registry;
    DataManager& m_dataManager;
};