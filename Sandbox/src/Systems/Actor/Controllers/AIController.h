#pragma once
#include <random>
#include "Game/Actor/Controllers/ActorController.h"
#include "Game/Commands/ActorCommands.h" 

/**/
class AIController : public ActorController
{
public:
    // AIController does not need an InputHandler
    AIController(Engine::ECS::Entity& target) : ActorController(target, ControllerType::AI) {}

    // Mark destructor as override
    ~AIController() {}

    // Implement the pure virtual update method from the base class
    void update() override
    {   
    }
    //AIHandler& m_handler;
};