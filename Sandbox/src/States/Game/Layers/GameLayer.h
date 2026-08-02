#pragma once
#include "Penguin.h"
#include "Systems/World/GameArea.h"

struct MoveableShape
{
    pgn::RenderShape* shape;
    pgn::PhysicsBody* body;
    pgn::Vector2 velocity = {0,0};
    float speed = 400;
};

class GameLayer : public pgn::Layer
{
public:
    GameLayer(const std::string name);
    virtual ~GameLayer() override;

    void OnEnter() override;
    void OnEvent(pgn::Event& e) override;
    void OnUpdate(pgn::Timestep ts) override;
    void OnRender(float alpha) override;

private:
    pgn::CameraController m_camera;
    pgn::Interpolated<float> textsize{1.f};
    AreaSystem m_areaSystem;
    pgn::PhysicsWorld m_world;

    std::vector<MoveableShape> m_shapes;
    pgn::Text text;
    pgn::LineShape line;
    pgn::Animator anim;
    pgn::Sprite sprite;
    pgn::RectangleShape rect;
    pgn::RectangleShape rect2;
    pgn::CircleShape circle;
    pgn::CircleShape circle2;
    pgn::CircleShape circle3;
};