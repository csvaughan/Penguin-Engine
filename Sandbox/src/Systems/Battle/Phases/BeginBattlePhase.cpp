/*#include "Game/Battle/Phases/BeginBattlePhase.h"
#include "Game/Battle/Combatant.h"
#include "Game/Battle/BattleGrid.h"
#include "Game/Battle/Phases/PlanningPhase.h" 
#include "Engine.h"
#include <random>
#include <chrono>

// Static RNG setup (can be moved to a a general utility class or global scope if preferred)
static std::mt19937 s_rng(std::chrono::steady_clock::now().time_since_epoch().count());
static std::uniform_int_distribution<int> s_coinDist(0, 1); // 0 for heads, 1 for tails

BeginBattlePhase::BeginBattlePhase(BattleGrid& grid, std::vector<Engine::ECS::Entity>& combatants)
:   IBattlePhase(grid, combatants, "Setup Phase")
{
    std::cout<< "Begin Battle Phase." << std::endl;

    // Initial actor placement (unrelated to coin flip logic)
    moveActortoStartPos(m_combatants[0], { 6, 4});
    moveActortoStartPos(m_combatants[1], {10, 4});
    m_completed = true; 
}

void BeginBattlePhase::handleInput(const sf::Event& event, Engine::Camera& cam)
{
    // No direct input handling in this phase for now
}

void BeginBattlePhase::update(float dt)
{
    m_completed = true; 
}

// Implement the draw method for BeginBattlePhase
void BeginBattlePhase::render(Engine::Camera& cam) 
{
   
}

pgn::Scope<IBattlePhase> BeginBattlePhase::transition()
{
    if (m_completed) {
        std::cout << "Transitioning to Planning Phase." << std::endl;
        return std::make_unique<PlanningPhase>(m_grid, m_combatants, Team::Player);
    }
    return nullptr; // Stay in this phase if not completed
}

void BeginBattlePhase::moveActortoStartPos(Engine::ECS::Entity& c, sf::Vector2i pos)
{ 
    c.GetTransform().position = m_grid.gridToWorld(pos);
    m_grid.occupyTile(pos, &c);
}*/