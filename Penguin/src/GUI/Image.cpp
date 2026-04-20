/*#include "GUI/Image.h"

namespace Engine
{
    namespace GUI
    {
        Image::Image(const sf::Vector2f& position, const sf::Vector2f& size, sf::Sprite& sprite)
            : GUIElement(position, size), m_sprite(sprite)
        {
            // The Image element's position should align with the sprite's position
            // Since m_sprite is an external reference (e.g., actor's sprite), we don't
            // want to change its position directly here, but we ensure the GUIElement's
            // position reflects where it's conceptually located on the GUI.
            // The actual drawing of the sprite is handled in draw().
        }

        void Image::update(float deltaTime)
        {
            // The Animation class (which lives within Actor) is responsible for updating m_sprite's
            // texture rectangle. This GUI element just displays it.
            // No direct update needed here for the sprite's internal animation state.
            // You might update internal GUIElement state (e.g., m_state) if this were interactive.
        }

        bool Image::handleEvent(const sf::Event& event, const sf::Vector2f& mousePos)
        {
            // For a simple display image, you might not need to handle events.
            // But if you want it to be clickable or highlight on hover, you'd add logic here.
            return false; // Event not consumed
        }

        void Image::draw(sf::RenderTarget& target, sf::RenderStates states) const
        {
            if (!m_visible) return;

            // Apply the GUIElement's transform (position, rotation, scale)
            // This allows you to position the Image element on the GUI.
            states.transform *= getTransform();

            // Draw the underlying sprite.
            // The sprite's own position is generally managed by the Actor.
            // Here, we're drawing the sprite, but it will appear at the GUIElement's position
            // due to the applied transform.
            // We need to temporarily adjust the sprite's position for drawing relative to the GUI element.
            // This is a bit tricky if the sprite's position is also managed externally (e.g. by Actor).

            // A more robust way: Create a temporary sprite for drawing
            sf::Sprite tempSprite = m_sprite;
            tempSprite.setPosition(getPosition()); // Position the temp sprite at the GUI element's position

            // Apply any scaling from GUIElement's size to the sprite's texture rect bounds
            // Assuming m_sprite's texture rect is already set by the Animation class
            float scaleX = m_size.x / tempSprite.getGlobalBounds().width;
            float scaleY = m_size.y / tempSprite.getGlobalBounds().height;
            tempSprite.setScale(scaleX, scaleY); // Scale the temp sprite to fit the GUIElement's size

            target.draw(tempSprite, states);
        }
    }
}*/