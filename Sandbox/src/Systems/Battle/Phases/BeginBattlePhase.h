#pragma once
#include "IBattlePhase.h"
#include "Engine.h"

class BeginBattlePhase : public IBattlePhase
{
public:
    BeginBattlePhase(BattleGrid& grid, std::vector<Engine::ECS::Entity>& combatants);

    void handleInput(const sf::Event&, Engine::Camera&) override;
    void update(float dt) override;
    void render(Engine::Camera&) override; 

    pgn::Scope<IBattlePhase> transition() override;

private:
    void moveActortoStartPos(Engine::ECS::Entity&, sf::Vector2i);
    void startCoinFlip(); // New method to initiate the flip
    void endCoinFlip();   // New method to handle flip completion
};