#pragma once
#include "Game/Battle/Phases/IBattlePhase.h"
#include "Engine.h"

class EndBattlePhase : public IBattlePhase
{
public:
    // Now takes the BattleController reference
    EndBattlePhase(BattleGrid& grid, std::vector<Engine::ECS::Entity>& combatants, Team team) 
    : IBattlePhase(grid, combatants, "Shutdown Phase"), m_winningTeam(team)
    {
        std::cout<< "Shutdown Phase." << std::endl;
    }
    void handleInput(const sf::Event&, Engine::Camera&) override {}
    void update(float) override{}
    void render(Engine::Camera&) override{}
    pgn::Scope<IBattlePhase> transition() override {return nullptr;}

private:
    Team m_winningTeam;
};