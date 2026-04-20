/*#include "Game/Battle/Phases/ActionPhase.h"
#include "Game/Commands/ActorCommands.h"
#include "Game/Battle/BattleGrid.h"

//helper function to get direction from vector
inline Direction calculateDirection(sf::Vector2f direction)
{
    if (std::abs(direction.x) > std::abs(direction.y)) {return (direction.x > 0) ? Direction::Right : Direction::Left;} 
    else {return (direction.y > 0) ? Direction::Down : Direction::Up;}
}

ActionPhase::ActionPhase(BattleGrid& grid, std::vector<Engine::ECS::Entity>& combatants, std::queue<CombatantAction> actions, Team team)
    : IBattlePhase(grid, combatants, "Action Phase"), 
    m_combatantActions(actions), 
    m_team(team), 
    m_winningTeam(Team::Default)
{
    std::cout << "Begin Action Phase." << std::endl;
}

ActionPhase::~ActionPhase()
{
    std::cout << "Action Phase Deactivated." << std::endl;
}

void ActionPhase::update(float dt)
{
    if (!m_completed) 
    {
        if (m_combatantActions.empty())
        {
            std::cout << "All actor paths have been processed." << std::endl;
            m_completed = true; 
            return;
        }

        CombatantAction& currentAction = m_combatantActions.front();
        m_grid.freeTile(currentAction.path.front());
        m_grid.occupyTile(currentAction.path.back(), currentAction.combatant);
        processMovement(currentAction.path, *currentAction.combatant);

        // If the current actor's path is now complete
        if (currentAction.path.empty())
        {   
            if(currentAction.target){executeAttack(*currentAction.combatant, *currentAction.target);}
            m_combatantActions.pop();
            removeDefeatedCombatants();
            testForWin();
        } 
    }
}

pgn::Scope<IBattlePhase> ActionPhase::transition()
{
    if (m_winningTeam == Team::Default)
    {
        Team newTeam = (m_team == Team::Player) ? Team::Enemy : Team::Player; //switch active team
        return std::make_unique<PlanningPhase>(m_grid, m_combatants, newTeam);
    }
    else
    {
        return std::make_unique<EndBattlePhase>(m_grid, m_combatants, m_winningTeam);
    }
}

void ActionPhase::processMovement(GridPath& path, Engine::ECS::Entity& combatant)
{
    sf::Vector2f targetTilePos = m_grid.gridToWorld(path.front());
    sf::Vector2f directionToTarget = targetTilePos - combatant.GetTransform().position;
    float distanceToTarget = Engine::Math::Vector::Magnitude(directionToTarget);
    float arrivalThreshold = 4.f; 
   
    //Check if the actor has arrived at the current target tile segment
    if (distanceToTarget <= arrivalThreshold) 
    {
        combatant.GetTransform().position = targetTilePos; // Snap actor precisely to the tile center 
        path.pop_front();

        //Check if this was the last tile in the path for current actor
        if (path.empty()) 
        {
            ActorCommand::Stop(combatant).execute();
            std::cout << "Actor " << combatant.GetID() << " path complete." << std::endl;
        }
        return;
    }

    //If not arrived at current target tile, set velocity to move towards it
    ActorCommand::Move(combatant, calculateDirection(Engine::Math::Vector::Normalize(directionToTarget))).execute();
}

void ActionPhase::executeAttack(Engine::ECS::Entity& attacker, Engine::ECS::Entity& target)
{
    CombantantState targetState = target.GetComponent<CombatantComponent>().state;
    Team targetTeam = target.GetComponent<CombatantComponent>().team;
    if (targetState == CombantantState::Defeated) {return;}

    auto& targetStats = target.GetComponent<ActorAttributesComponent>().stats;
    auto& attackerStats = attacker.GetComponent<ActorAttributesComponent>().stats;

    // Trigger Animations
    ActorCommand::Attack(attacker, calculateDirection(target.GetTransform().position - attacker.GetTransform().position)).execute();
    ActorCommand::Hurt(target).execute();
    // Apply Damage
    targetStats.health.current -= attackerStats.attack;
    std::cout << attacker.GetID() << " did " << attackerStats.attack << " damage to " << target.GetID() << std::endl;
    std::cout << target.GetID() << " health: " << targetStats.health.current << std::endl;

    if(targetStats.health.current <=0)
    {
        if(targetTeam == Team::Player) {targetStats.health.current = 0;}
        targetState = CombantantState::Defeated;
    }
}

void ActionPhase::removeDefeatedCombatants()
{
    m_combatants.erase(
    std::remove_if(m_combatants.begin(), m_combatants.end(),[&](Engine::ECS::Entity& combatant) 
    {
        if (combatant.GetComponent<CombatantComponent>().state == CombantantState::Defeated) 
        {
            m_grid.freeTile(m_grid.getCombatantTilePosition(&combatant));
        }
        return false; // Keep
    }),m_combatants.end());
}

void ActionPhase::testForWin()
{
    int playerCount, enemyCount;
    for (Engine::ECS::Entity& c : m_combatants)
    {
        (c.GetComponent<CombatantComponent>().team == Team::Player) ? playerCount++ : enemyCount++;
    }
    if (playerCount == 0 || enemyCount == 0)
    {
        m_winningTeam = (enemyCount == 0) ? Team::Player : Team::Enemy;
        std::cout << "Team: " << static_cast<int>(m_winningTeam) << "wins" << std::endl;
        m_completed = true;
    }
}*/