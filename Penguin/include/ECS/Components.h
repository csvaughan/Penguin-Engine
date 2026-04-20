#pragma once
#include "pgpch.h"
#include "Renderer/Color.h"
#include "Renderer/Sprite.h"

namespace pgn
{
    namespace ECS
    {
        class Component {};
    }

    struct TagComponent : ECS::Component
    {
        std::string tag;
        TagComponent() = default;
        TagComponent(std::string t) : tag(t){}
    };

    struct TransformComponent : public ECS::Component 
    {
        Vector2 position = { 0.0f, 0.0f };
        Vector2 scale = { 1.0f, 1.0f };
        Vector2 origin = { 0.0f, 0.0f }; 
        float rotation = 0.0f;
        TransformComponent() = default;
    };

    struct SpriteComponent : public ECS::Component 
    {
        Sprite sprite;
        Color color;
        uint8_t layer;
        SpriteComponent (Color c = Color::White, uint8_t l = 0): color(c), layer(l) {}
    };

    struct MovementComponent : public ECS::Component 
    {
        Vector2 velocity = {0,0};
        float speed = 100.0f;
        float acceleration;
        MovementComponent(Vector2 v = {0,0}, float s = 100) : velocity(v), speed(s){} 
    };

    /*
    struct BoxCollider2DComponent : public ECS::Component 
    {
        sf::RectangleShape shape;
        bool isTrigger;

        BoxCollider2DComponent (bool t = false) : shape({1,1}), isTrigger(t)
        {
            auto bounds = shape.getGlobalBounds();// set origin to center
            shape.setOrigin(bounds.width/2, bounds.height/2);
        }
        BoxCollider2DComponent (const sf::Vector2f& size, bool t = false) : shape({size}), isTrigger(t)
        {
            shape.setOrigin(size.x / 2.0f, size.y / 2.0f);
        }
    }; 

    struct CircleCollider2DComponent : public ECS::Component 
    {
        sf::CircleShape shape;
        bool isTrigger = false;

        CircleCollider2DComponent () : shape({1}){}
        CircleCollider2DComponent (const float size) : shape({size}){}
    };*/ 
}