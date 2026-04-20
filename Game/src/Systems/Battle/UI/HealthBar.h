#pragma once
#include <algorithm> 
#include "Game/Battle/Combatant.h"
#include "GUI.h"

class HealthBar : public Engine::GUI::GUIElement
{
public:
    HealthBar(const Engine::ECS::Entity c);

    void update(float deltaTime) override;
    bool handleEvent(const sf::Event& event, const sf::Vector2f& mousePos) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    const Engine::ECS::Entity m_combatant;
    sf::RectangleShape m_backgroundBar;
    sf::RectangleShape m_healthFill;
};

HealthBar::HealthBar(const Engine::ECS::Entity c) : Engine::GUI::GUIElement(c.GetTransform().position, {70.f, 8.f}), m_combatant(c)
{
    // --- Setup Background Bar ---
    m_backgroundBar.setSize(m_size);
    m_backgroundBar.setFillColor(sf::Color(50, 50, 50, 210)); // Dark grey, semi-transparent
    m_backgroundBar.setOutlineColor(sf::Color::Black);
    m_backgroundBar.setOutlineThickness(1.f);
    // --- Setup Health Fill ---
    m_healthFill.setSize(m_size);
    m_healthFill.setFillColor(sf::Color::Green); // Default to green

    // Run an initial update to set the correct health and position from the start.
    update(0.f);
}

// --- Update Method ---
void HealthBar::update(float deltaTime)
{
    if (m_combatant.GetComponent<CombatantComponent>().state == CombantantState::Defeated) {
        m_visible = false;
        return;
    }
    m_visible = true;

    // Center the health bar above the combatant's actor sprite.
    const float ACTOR_PADDING = 50.f;
    sf::Vector2f actorPosition = m_combatant.GetTransform().position;
    float xPos = actorPosition.x - (m_size.x / 2.f);
    float yPos = actorPosition.y - m_size.y - ACTOR_PADDING;
    setPosition(xPos, yPos);

    // --- Update Health Percentage ---
    auto& health = m_combatant.GetComponent<ActorAttributesComponent>().stats.health;
    float maxHp = static_cast<float>(health.max);
    float currentHp = static_cast<float>(health.current);

    // Clamp health and calculate percentage to prevent visual errors.
    currentHp = std::max(0.f, currentHp);
    float healthPercent = (maxHp > 0) ? (currentHp / maxHp) : 0.f;

    // Update the width of the health fill rectangle based on the percentage.
    m_healthFill.setSize({ m_size.x * healthPercent, m_size.y });

    // --- Update Color based on Health ---
    if (healthPercent > 0.5f) {
        m_healthFill.setFillColor(sf::Color(76, 175, 80)); // Material Green
    } else if (healthPercent > 0.25f) {
        m_healthFill.setFillColor(sf::Color(255, 235, 59)); // Material Yellow
    } else {
        m_healthFill.setFillColor(sf::Color(211, 47, 47)); // Material Red
    }
}

// --- Event Handling ---
bool HealthBar::handleEvent(const sf::Event& event, const sf::Vector2f& mousePos)
{
    if (!m_enabled || !m_visible) {return false;}

    // Update the internal state if the mouse is hovering over the bar.
    // This could be used to show a tooltip with exact HP values.
    if (getGlobalBounds().contains(mousePos)) {
        m_state = Engine::GUI::UIState::Hovered;
    } else {
        m_state = Engine::GUI::UIState::Normal;
    }

    // A health bar doesn't typically "consume" an event, so we return false to allow other GUI elements to process it.
    return false;
}

// --- Draw Method ---
void HealthBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (!m_visible) {return;}

    // Apply the element's transform (position, rotation, scale) to the render states.
    states.transform *= getTransform();

    // Draw the components. The background is drawn first, then the fill on top.
    target.draw(m_backgroundBar, states);
    target.draw(m_healthFill, states);
}