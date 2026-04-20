#pragma once
#include "Battle.h"

class BattleData
{
public:
    BattleData(Battle& battle) : m_battle(battle){}
    ~BattleData(){}

    const std::vector<Engine::ECS::Entity>& getCombatants()const {return m_battle.m_combatants;}
    const BattleGrid& getGrid() const {return m_battle.m_grid;}
    const std::string& getCurrentPhaseName() const {return m_battle.m_currentPhase->getName();}
    bool isBattleOver() const {return !m_battle.m_running;} 

private:
    Battle& m_battle;
};