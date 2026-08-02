#pragma once
#include "Battle.h"
#include "BattleData.h"

class BattleSystem
{
public:
    BattleSystem(Engine::ECS::Registry& ecs) : m_registry(ecs){}
    ~BattleSystem() {}

    void update(float dt);
    bool isBattleRunning() {return (m_battle != nullptr);}

    void onBattleCreation();

    BattleData getBattleData(){ return BattleData(*m_battle.get());}
    void render(Engine::Camera& cam){if(m_battle) m_battle->render(cam);}

    void onEvent(sf::Event& e, Engine::Camera& c){if(m_battle) m_battle->handleInput(e, c);}

private:

    void createBattle();
    void testForBattle();

    Engine::ECS::Registry& m_registry;
    pgn::Scope<Battle> m_battle;
};
