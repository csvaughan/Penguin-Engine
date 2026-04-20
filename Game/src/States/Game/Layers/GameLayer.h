#pragma once
#include "Penguin.h"
#include "Systems/World/GameArea.h"

struct MoveableShape
{
    pgn::RenderShape* shape;
    pgn::Vector2 velocity = {0,0};
    float speed = 400;
};

class GameLayer : public pgn::Layer
{
    public:
        GameLayer(const std::string name) : pgn::Layer(name), m_camera(App().GetWindow().GetWindowWidth(),App().GetWindow().GetWindowHeight()), m_areaSystem(m_camera) {}
        ~GameLayer(){}

        void OnEnter() override 
        {
            text.setFont(App().GetFont("default_font"));
            text.setString("Hello World");
            text.setPosition({500,200});
            text.setColor(pgn::Color::Cyan);
            text.centerOrigin();

            textsize.setTransitionType(pgn::Transition::EaseOutElastic);
            textsize.setDuration(3);
            textsize.setValue(2);

            rect.setPosition({400,400});
            rect.setColor(pgn::Color::Yellow);
            rect.centerOrigin();

            rect2.setPosition({600,200});
            rect2.setColor(pgn::Color::Green);
            rect2.setOutlineThickness(1);
            MoveableShape r2({&rect2, {(float)pgn::Random::Sign(), (float)pgn::Random::Sign()}, (float)pgn::Random::Roll(5)*100});
            m_shapes.push_back(r2);
            PGN_INFO("Shape speed {} ", r2.speed);
            
            circle.setPosition({800, 400});
            circle.setColor(pgn::Color::Blue);
            MoveableShape c({&circle, {(float)pgn::Random::Sign(), (float)pgn::Random::Sign()}, (float)pgn::Random::Roll(5)*100});
            m_shapes.push_back(c);
            PGN_INFO("Shape speed {} ", c.speed);

            circle2.setPosition({1000, 500});
            circle2.setColor(pgn::Color::Red);
            circle2.setOutlineThickness(1);
            MoveableShape c2({&circle2, {(float)pgn::Random::Sign(), (float)pgn::Random::Sign()}, (float)pgn::Random::Roll(5)*100});
            m_shapes.push_back(c2);
            PGN_INFO("Shape speed {} ", c2.speed);

            sprite.setPosition({400, 200});
            sprite.setScale({2,2});
            anim.play(pgn::Animation::CreateFromSheet(App().GetTexture("WD"), 48, 48, 0.3f));

            line.setColor(pgn::Color::FromHex("#6c1dc7"));
            line.setPoint1({200,300});
            line.setPoint2({100,100});
            line.setOutlineThickness(10);
            line.centerOrigin();
        }

        void OnEvent(pgn::Event& e) override
        {

        }

        void OnUpdate(pgn::Timestep ts) override
        {
            float winWidth  = App().GetWindow().GetWindowWidth();
            float winHeight = App().GetWindow().GetWindowHeight();

            m_camera.setViewportSize(winWidth, winHeight);

            for (auto& s : m_shapes)
            {
                pgn::Vector2 pos = s.shape->getPosition();

                // 1. Check X Bounds
                if(pos.x > winWidth)
                {
                    s.velocity.x = -1.0f;
                    s.shape->setPosition({winWidth, pos.y}); 
                }
                else if(pos.x < 0)
                {
                    s.velocity.x = 1.0f;
                    s.shape->setPosition({0, pos.y});
                }
                if(pos.y > winHeight)
                {
                    s.velocity.y = -1.0f;
                    s.shape->setPosition({pos.x, winHeight});
                }
                else if(pos.y < 0)
                {
                    s.velocity.y = 1.0f;
                    s.shape->setPosition({pos.x, 0});
                }

                pgn::Vector2 currentPos = s.shape->getPosition();
                pgn::Vector2 movement = s.velocity * s.speed * ts.GetSeconds();
                
                s.shape->setPosition(currentPos + movement);
            }

            rect.setRotation(rect.getRotation() + 50 * ts.GetSeconds());

            
            if (textsize.isFinished())
                textsize = (textsize.getValue()*-1);
            
            text.setScale({textsize, textsize});

            line.setRotation(line.getRotation() - 50 * ts.GetSeconds());
            
            anim.update(ts.GetSeconds(), sprite);
            
            if(pgn::Input::GetKeyDown(pgn::KeyCode::A))
            {
                rect.setPosition(m_camera.screenToWorld({pgn::Random::Uniform<float>(0, winWidth), pgn::Random::Uniform<float>(0, winHeight)}));
                std::println("Teleported to ({}, {})", rect.getPosition().x, rect.getPosition().y);
            }
            if(pgn::Input::GetKeyDown(pgn::KeyCode::P))
            {
                pgn::AudioSystem::Play(App().GetAudio("test"));
            }

            if(pgn::Input::GetKeyDown(pgn::KeyCode::MouseButtonLeft))
            {
                std::println("Mouse Pos ({}, {})", pgn::Input::GetMousePos().x, pgn::Input::GetMousePos().y);
            }

            pgn::Vector2 movement = pgn::Vector2::Zero();

            if (pgn::Input::GetKey(pgn::KeyCode::Up))    movement = pgn::Vector2::Up();
            if (pgn::Input::GetKey(pgn::KeyCode::Down))  movement = pgn::Vector2::Down();
            if (pgn::Input::GetKey(pgn::KeyCode::Left))  movement = pgn::Vector2::Left();
            if (pgn::Input::GetKey(pgn::KeyCode::Right)) movement = pgn::Vector2::Right();

            // Normalize so diagonal movement isn't faster
            if (movement.Length() > 0) { movement.Normalize(); }

            // Call move with the speed and delta time
            m_camera.move(movement * ts.GetSeconds());
            m_camera.onUpdate(ts.GetSeconds());
        }

        void OnRender(float alpha) override
        {
            pgn::Renderer::BeginScene(m_camera.getCamera());
            pgn::Renderer::Submit(rect);
            pgn::Renderer::Submit(rect2);
            pgn::Renderer::Submit(circle);
            pgn::Renderer::Submit(circle2);
            pgn::Renderer::Submit(sprite);
            pgn::Renderer::Submit(line);
            pgn::Renderer::Submit(text);

            pgn::Renderer::EndScene();
        }

    private:

        pgn::CameraController m_camera;

        pgn::Interpolated<float> textsize{1.f};

        AreaSystem m_areaSystem;

        std::vector<MoveableShape> m_shapes;
        pgn::Text text;
        pgn::LineShape line;
        pgn::Animator anim;
        pgn::Sprite sprite;
        pgn::RectangleShape rect;
        pgn::RectangleShape rect2;
        pgn::CircleShape circle;
        pgn::CircleShape circle2;
};