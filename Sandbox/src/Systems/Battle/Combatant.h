#pragma once
#include "Engine.h"
#include "Game/Actor/Attributes.h"

enum class Team{ Default=0, Player, Enemy};

enum class CombantantState {Active, Defeated, Stunned};

struct CombatantComponent : public Engine::ECS::Component
{
    Team team;
    CombantantState state;
    //std::vector<HangingEffects> m_effectes;

    CombatantComponent() : team(Team::Default), state(CombantantState::Active){}
    CombatantComponent(Team t, CombantantState cs) : team(t), state(cs){}  
};
