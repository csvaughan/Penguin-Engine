/*#include "Game/Battle/Phases/PlanningPhase.h"
#include "Game/Battle/Phases/ActionPhase.h"
#include "Game/Battle/BattleGrid.h"
#include "Engine.h"
#include "Game/Actor/Attributes.h"

PlanningPhase::PlanningPhase(BattleGrid& grid, std::vector<Engine::ECS::Entity>& combatants, Team team) 
: IBattlePhase(grid, combatants, "Planning Phase"), m_selectedCombatant(nullptr), m_currentTeam(team)
{
    std::cout<< "Begin Planning Phase." << std::endl;
}

PlanningPhase::~PlanningPhase() 
{
    m_grid.clearAllHighlights(); // Clear highlights when phase ends
    std::cout << "Planning Phase Deactivated." << std::endl;
}

void PlanningPhase::handleInput(const sf::Event& event, Engine::Camera& cam)
{
    cam.setPosition(m_grid.getCenter());

    switch (event.type)
    {
    case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::Enter){ m_completed = true;} //temp until UI is implimented
        break;
        
    case sf::Event::MouseMoved:
        onMouseHover(event, cam.getView());
        break;
    
    case sf::Event::MouseButtonPressed:
        onMouseClick(event, cam.getView());
        break;

    default:
        break;
    }
}

void PlanningPhase::update(float dt)
{
    if (m_completed) return;

    //add camera update here. Change camera based on view or something
    for(auto& action : m_plannedActions) {highlightPath(action.path);}
}

pgn::Scope<IBattlePhase> PlanningPhase::transition()
{
    std::queue<CombatantAction> actions;
    for(auto&  action : m_plannedActions) {actions.emplace(action);}
    return std::make_unique<ActionPhase>(m_grid, m_combatants, actions, m_currentTeam);
}

void PlanningPhase::highlightMovableRange(Engine::ECS::Entity* combatant)
{
    const int highlightRadius = combatant->GetComponent<ActorAttributesComponent>().stats.movementRange; 
    sf::Vector2i startPos = m_grid.getCombatantTilePosition(combatant);
    
    for (int y = startPos.y - highlightRadius; y <= startPos.y + highlightRadius; ++y)
    {
        for (int x = startPos.x - highlightRadius; x <= startPos.x + highlightRadius; ++x)
        {
            sf::Vector2i currentTile = {x, y};

            if (!m_grid.isValidGridPos(currentTile)) continue;

            if (Engine::Math::Distance::Manhattan(currentTile, startPos) <= highlightRadius)
            {
                if (m_grid.isTileWalkable(currentTile)) 
                {
                    m_grid.highlightTile(currentTile, sf::Color(0, 255, 0, 100)); // Lighter Green for movable tiles
                }
            }
        }
    }
}

void PlanningPhase::highlightPath(GridPath& path)
{                                                
    if (!path.empty())
    { 
        sf::Vector2i startPos = path.front(), targetTile = path.back();

        for (const auto& tilePos : path)
        {
            if (tilePos != startPos && tilePos != targetTile)
            {
                m_grid.highlightTile(tilePos, sf::Color(255, 165, 0, 180)); // Orange for path
            }
        }
        m_grid.highlightTile(targetTile, sf::Color::Red);
    }
}

void PlanningPhase::selectCombatant(Engine::ECS::Entity* combatant) 
{
    if (m_selectedCombatant != combatant) 
    {
        m_grid.clearAllHighlights();
        m_selectedCombatant = combatant;

        //delete current combatant path if it exists
        for (auto it = m_plannedActions.begin(); it != m_plannedActions.end();)
            (it->combatant == combatant) ? m_plannedActions.erase(it) : it++;

        m_targetTile = {-1, -1};
        highlightMovableRange(combatant);
    }
}

void PlanningPhase::deselectCombatant() 
{
    if (m_selectedCombatant) 
    {
        m_selectedCombatant = nullptr;
        m_targetTile = {-1, -1};
        m_hoveredTile = {-1, -1}; // Reset hovered tile as well
        m_grid.clearAllHighlights(); // Clear all highlights when no combatant is selected
    }
}

void PlanningPhase::onMouseHover(const sf::Event& event, const sf::View& view)
{
    sf::Vector2i newHoveredGridPos = m_grid.worldToGrid(m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window), view));
    // Check if the hovered tile has changed
    if (newHoveredGridPos != m_hoveredTile)
    {
        // Unhighlight the OLD hovered tile if it was valid and not the target tile
        if (m_grid.isValidGridPos(m_hoveredTile) && m_hoveredTile != m_targetTile) 
        {
            m_grid.unhighlightTile(m_hoveredTile);
        }
        // Update to the NEW hovered tile
        m_hoveredTile = newHoveredGridPos;

        // Highlight the NEW hovered tile if it is valid and not the target tile
        if (m_grid.isValidGridPos(m_hoveredTile) && m_hoveredTile != m_targetTile) 
        {
            m_grid.highlightTile(m_hoveredTile, sf::Color(0, 255, 255, 120)); // Cyan for hover
        }
    }

    // If an combatant is selected, update path/range highlights on mouse move
    if (m_selectedCombatant) 
    {
        m_grid.clearAllHighlights(); //clear all highlights first
        highlightMovableRange(m_selectedCombatant); 
        sf::Vector2i CombatantPos = m_grid.getCombatantTilePosition(m_selectedCombatant);
        float range = m_selectedCombatant->GetComponent<ActorAttributesComponent>().stats.movementRange;
        Team team = m_selectedCombatant->GetComponent<CombatantComponent>().team;
        
        // If hovered tile is valid and different from combatant's current tile, highlight path
        if (m_grid.isValidGridPos(m_hoveredTile) && m_hoveredTile != CombatantPos ) 
        {
            m_currentPath = BattlePathfinder::findPath(m_grid, CombatantPos, m_hoveredTile, range, team);
            highlightPath(m_currentPath);
        }
    }
}

void PlanningPhase::onMouseClick(const sf::Event& event, const sf::View& view)
{
    if (event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i clickedGridPos = m_grid.worldToGrid(m_window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y}, view));

        if (!m_grid.isValidGridPos(clickedGridPos)) // Clicked outside grid, deselect any active combatant/plan
        {   
            deselectCombatant();
            return;
        }
        Engine::ECS::Entity* clickedCombatant = m_grid.getTile(clickedGridPos).occupyingCombatant;

        if (clickedCombatant && clickedCombatant->GetComponent<CombatantComponent>().team == m_currentTeam) // Clicked on an combatant
        {
            selectCombatant(clickedCombatant);
        }
        else // Clicked on an empty tile
        {
            if (m_selectedCombatant) // If an combatant is currently selected, this click is for movement
            {
                if (m_grid.isTileWalkable(clickedGridPos))
                { 
                    m_plannedActions.push_back({m_selectedCombatant, m_currentPath}); // Add tile to planned destination
                }
                if (m_grid.getTile(clickedGridPos).occupyingCombatant) //if clicked on enemy, remove last step in path and add target
                {
                    sf::Vector2i combatantPos = m_grid.getCombatantTilePosition(m_selectedCombatant);
                    
                    // Check if the clicked combatant is within the movement range
                    if (Engine::Math::Distance::Manhattan(clickedGridPos, combatantPos) <= m_selectedCombatant->GetComponent<ActorAttributesComponent>().stats.movementRange)
                    {
                        Engine::ECS::Entity* target = m_grid.getTile(clickedGridPos).occupyingCombatant;
                        m_currentPath.pop_back(); // Remove the last step in the path to the target's tile
                        m_plannedActions.push_back({m_selectedCombatant, m_currentPath, target});
                    }
                }
                deselectCombatant(); // Deselect after planning a move
            }
            else // No combatant selected, clicked on an empty tile
            {
                deselectCombatant();
            }
        }   
    }
    if (event.mouseButton.button == sf::Mouse::Right)
    {
        deselectCombatant();
    }
}*/