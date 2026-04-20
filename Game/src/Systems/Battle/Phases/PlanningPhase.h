#pragma once
#include "Game/Battle/Phases/IBattlePhase.h"
#include "Game/Battle/BattlePathfinder.h"
#include "Game/Battle/Combatant.h"
#include "Game.h"

namespace Engine {class Camera; class Window;}

struct CombatantAction 
{
    Engine::ECS::Entity* combatant;
    GridPath path; 
    Engine::ECS::Entity* target;

    CombatantAction(Engine::ECS::Entity* c, GridPath p, Engine::ECS::Entity* t = nullptr)
        : combatant(c), path(p), target(t){}
};

class PlanningPhase : public IBattlePhase
{
public:
    PlanningPhase(BattleGrid&, std::vector<Engine::ECS::Entity>&, Team);

    ~PlanningPhase();

    void handleInput(const sf::Event&, Engine::Camera&) override;
    void update(float) override;
    void render(Engine::Camera&) override {}
    pgn::Scope<IBattlePhase> transition() override;

private:
    void highlightMovableRange(Engine::ECS::Entity*);
    void highlightPath(GridPath&);
    void selectCombatant(Engine::ECS::Entity*);
    void deselectCombatant();
    void onMouseClick(const sf::Event&, const sf::View&);
    void onMouseHover(const sf::Event&, const sf::View&);

private:
    Team m_currentTeam;
    Engine::Window& m_window = Game::get().getWindow();
    Engine::ECS::Entity* m_selectedCombatant;
    sf::Vector2i m_hoveredTile = {-1, -1};
    sf::Vector2i m_targetTile = {-1, -1};
    GridPath m_currentPath;
    std::vector<CombatantAction> m_plannedActions;
};