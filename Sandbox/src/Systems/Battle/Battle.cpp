/*#include "Game/Actor/Controllers/PlayerController.h" 
#include "Game/Battle/Battle.h"
#include "Game/Battle/Phases/EndBattlePhase.h"
#include "Game/Battle/Phases/BeginBattlePhase.h"


Battle::Battle(std::vector<Engine::ECS::Entity> initialCombatants) 
:   m_running(true),
    m_grid(BattleGrid(12, 9, sf::Vector2f(72.0f, 72.0f), sf::Vector2f(200.0f, 100.0f)))
{
    initBattle(initialCombatants); 
}

Battle::~Battle()
{
    for (auto& e : m_combatants) 
    {
       e.GetComponent<ActorControllerComponent>().controller->setActive(true);
       e.RemoveComponent<CombatantComponent>();
    }
}

void Battle::initBattle(std::vector<Engine::ECS::Entity> initialCombatants) 
{
    for (auto& e : initialCombatants) 
    {
        //set controller to inactive
        e.GetComponent<ActorControllerComponent>().controller->setActive(false);
        //add actor to combatants by team
        Team team = (e.GetComponent<ActorAttributesComponent>().type == ActorType::Player) ? Team::Player : Team::Enemy;
        e.AddComponent(CombatantComponent(team, CombantantState::Active));
        m_combatants.push_back(e);
    }

    m_currentPhase = std::make_unique<BeginBattlePhase>(m_grid, m_combatants);
}

void Battle::endBattle()
{

}

void Battle::update(float dt)
{
    if (m_currentPhase)
    {
        m_currentPhase->update(dt);
        if(m_currentPhase->isCompleted()) {m_currentPhase = m_currentPhase->transition();}
    }
    else
    {
        endBattle();
    }
}

void Battle::handleInput(const sf::Event& e, Engine::Camera& c)
{
    if(m_currentPhase)
    {
        if (m_currentPhase) {m_currentPhase->handleInput(e, c);}
    }
}

void Battle::render(Engine::Camera& c)
{
    m_grid.render();
    m_currentPhase->render(c);
}*/