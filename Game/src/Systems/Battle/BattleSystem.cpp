/*#include "Game/Battle/BattleSystem.h"

void BattleSystem::update(float dt)
{
    if (m_battle && m_battle->isRunning())
    {
        m_battle->update(dt);
    }
    else
    {
        testForBattle();
    }  
}   

void BattleSystem::testForBattle()
{
    if(m_battle) {return;}

    // Define the detection range for starting a battle.
    constexpr float BATTLE_START_RANGE = 50.0f; // Example: 50 pixels
    constexpr float BATTLE_START_RANGE_SQ = BATTLE_START_RANGE * BATTLE_START_RANGE;

    // Get a view of all entities that have attributes and a transform.
    auto view = m_registry.getView<ActorAttributesComponent>();

    for (size_t i = 0; i < view.size(); ++i)
    {
        for (size_t j = i + 1; j < view.size(); ++j)
        {
            auto& e1 = view[i];
            auto& e2 = view[j];

            // Calculate squared distance
            sf::Vector2f displacement = e1.GetTransform().position - e2.GetTransform().position;
            float distanceSq = (displacement.x * displacement.x) + (displacement.y * displacement.y);

            // Check if the enemy is within the player's detection range
            if (distanceSq <= BATTLE_START_RANGE_SQ)
            {
                ActorType type1 = e1.GetComponent<ActorAttributesComponent>().type;
                ActorType type2 = e2.GetComponent<ActorAttributesComponent>().type;
                if (type1 == ActorType::Player && type2 == ActorType::Enemy)
                {
                    std::cout << "Player is in range of an enemy! Starting battle.\n";
                    
                    // m_battle = std::make_unique<Battle>(playerEntity, enemyEntity);
                    break; // Exit the inner (enemy) loop
                }
            }
        }
    }
}*/