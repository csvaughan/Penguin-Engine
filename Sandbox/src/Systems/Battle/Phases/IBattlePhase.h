#pragma once
#include "Engine.h"

class BattleGrid;
class Combatant;
namespace Engine {class Camera;}

class IBattlePhase
{
public:
    IBattlePhase(BattleGrid& grid, std::vector<Engine::ECS::Entity>& combatants ,std::string name = "Battle Phase") 
    : m_grid(grid), m_combatants(combatants), m_completed(false), m_name(name){}
    virtual ~IBattlePhase(){}

    virtual void handleInput(const sf::Event&, Engine::Camera&) = 0;
    virtual void update(float) = 0;
    virtual void render(Engine::Camera&) = 0;
    virtual pgn::Scope<IBattlePhase> transition() = 0;

    bool isCompleted() {return m_completed;}
    void complete() {m_completed = true;}

    const std::string& getName() const {return m_name;}
protected:
    BattleGrid& m_grid;
    std::vector<Engine::ECS::Entity>& m_combatants;
    bool m_completed;
    std::string m_name;
};