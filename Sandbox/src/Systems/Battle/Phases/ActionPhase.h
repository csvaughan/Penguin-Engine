#pragma once
#include "Game/Battle/Phases/PlanningPhase.h"
#include "Game/Battle/Phases/EndBattlePhase.h"
#include "Engine.h"

class ActionPhase : public IBattlePhase
{
public:
    ActionPhase(BattleGrid&, std::vector<Engine::ECS::Entity>&, std::queue<CombatantAction>, Team);
    ~ActionPhase();

    void handleInput(const sf::Event&, Engine::Camera&) override{}
    void update(float) override;
    void render(Engine::Camera&) override {}
    pgn::Scope<IBattlePhase> transition() override;

private:
    void processMovement(GridPath&, Engine::ECS::Entity&);
    void executeAttack(Engine::ECS::Entity&, Engine::ECS::Entity&);
    void removeDefeatedCombatants();
    void testForWin();

private:
    Team m_team;
    Team m_winningTeam;
    std::queue<CombatantAction> m_combatantActions;
    std::vector<Combatant*> m_defeatedCombatants;
};