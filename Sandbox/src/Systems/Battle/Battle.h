#pragma once
#include "Engine.h"
#include "Game/Battle/BattleGrid.h"
#include "Game/Battle/Phases/IBattlePhase.h"
#include "Game/Battle/Combatant.h"

namespace Engine {class Camera;}

class Battle
{
public:
    friend class BattleData;
    Battle(std::vector<Engine::ECS::Entity>);
    ~Battle();

    void handleInput(const sf::Event&, Engine::Camera&);
    void update(float);
    void render(Engine::Camera&);
    bool isRunning() {return m_running;}

private:
    void initBattle(std::vector<Engine::ECS::Entity>);
    void endBattle();

    bool m_running;

    Team m_currentTeam;
    Team m_winningTeam;
    BattleGrid m_grid;
    std::vector<Engine::ECS::Entity> m_combatants;
    pgn::Scope<IBattlePhase> m_currentPhase;
};