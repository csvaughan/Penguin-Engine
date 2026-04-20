#pragma once
#include "Engine.h"

enum class Direction {Up, Down, Left, Right};

enum class Action
{
    Idle,
	Walk,
	Attack,
    Hurt,
    Death
};

/// Type Attributes ///
enum class ActorType 
{
    Player,
    Enemy,
    NPC
};

enum class Race
{
    Default = 0,
    Human,
    Orc,
    Goblin,
    Undead,
    Dragon
};

enum class Class
{
    Warrior,
    Healer,
    Wizard
};

///Battle Attributes///

struct Health{
    unsigned int max;
    unsigned int current;

    Health(unsigned int health) : max(health), current(health) {}
};

struct Stats
{
    unsigned int attack;
    unsigned int defence;
    unsigned int movementRange;
    Health health;

    Stats() : defence(0), attack(0), movementRange(0), health(0){}

    Stats(unsigned int att, unsigned int def, unsigned int mr, unsigned int h) 
    : attack(att), defence(def), movementRange(mr), health(h){}
};

/// Full Attributes Component ///
struct ActorAttributesComponent : public Engine::ECS::Component
{
    ActorType type;
    Race race;
    Stats stats;
    bool alive;

    ActorAttributesComponent(ActorType t) : type(t), race(Race::Default), alive(true){}
};