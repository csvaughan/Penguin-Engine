#include "GameLayer.h"

GameLayer::GameLayer(const std::string name) 
    : pgn::Layer(name), m_camera(App().GetWindow().GetWindowSize()), m_areaSystem(m_camera), m_world({0.0f, 0.0f}) {} //

GameLayer::~GameLayer(){} //

void GameLayer::OnEnter()
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
    rect.setOpacity(100); 
    rect.centerOrigin(); 

    circle3.setColor(pgn::Color::Red);
    circle3.setRadius(25);
    circle3.setOpacity(.6); 
    circle3.centerOrigin();
    circle3.setPosition({400,400}); 
    circle3.setZIndex(3);

    float winWidth  = App().GetWindow().GetWindowWidth(); 
    float winHeight = App().GetWindow().GetWindowHeight(); 

    // ==========================================
    // 1. CREATE STATIC WORLD BOUNDARY WALLS
    // ==========================================
    // Creating static bodies just off-screen guarantees that our moving bodies 
    // will bounce back naturally without writing screen-clamping math.
    float wallThickness = 20.0f;

    // Top Wall
    pgn::PhysicsBody* topWall = m_world.CreatePhysicsBody({ winWidth / 2.0f, -wallThickness }, pgn::PhysicsBodyType::Static); 
    topWall->AddBoxCollider({ winWidth / 2.0f, wallThickness }, 0.0f);

    // Bottom Wall
    pgn::PhysicsBody* bottomWall = m_world.CreatePhysicsBody({ winWidth / 2.0f, winHeight + wallThickness }, pgn::PhysicsBodyType::Static); 
    bottomWall->AddBoxCollider({ winWidth / 2.0f, wallThickness }, 0.0f);

    // Left Wall
    pgn::PhysicsBody* leftWall = m_world.CreatePhysicsBody({ -wallThickness, winHeight / 2.0f }, pgn::PhysicsBodyType::Static); 
    leftWall->AddBoxCollider({ wallThickness, winHeight / 2.0f }, 0.0f);

    // Right Wall
    pgn::PhysicsBody* rightWall = m_world.CreatePhysicsBody({ winWidth + wallThickness, winHeight / 2.0f }, pgn::PhysicsBodyType::Static); 
    rightWall->AddBoxCollider({ wallThickness, winHeight / 2.0f }, 0.0f);


    // ==========================================
    // 2. INITIALIZE DYNAMIC SHAPES WITH COLLIDERS
    // ==========================================

    // Rectangle 2
    rect2.setPosition({600,200}); 
    rect2.setColor(pgn::Color::Blue); 
    rect2.setOutlineThickness(5); 
    rect2.centerOrigin(); 
    
    pgn::PhysicsBody* r2Body = m_world.CreatePhysicsBody(rect2.getPosition(), pgn::PhysicsBodyType::Dynamic); 
    r2Body->AddBoxCollider({ rect2.getWidth()/2, rect2.getHeight()/2 }, 1.0f); // Attach a physical box shape!

    pgn::Vector2 r2Vel = { (float)pgn::Random::Sign(), (float)pgn::Random::Sign() }; 
    float r2Speed = (float)pgn::Random::Roll(5) * 100; 
    r2Body->SetLinearVelocity(r2Vel * r2Speed); // Set initial velocity directly on the physical body

    m_shapes.push_back({ &rect2, r2Body });


    // Circle 1
    circle.setPosition({800, 400}); 
    circle.setColor(pgn::Color::Green); 
    
    pgn::PhysicsBody* c1Body = m_world.CreatePhysicsBody(circle.getPosition(), pgn::PhysicsBodyType::Dynamic); 
    c1Body->AddCircleCollider(circle.getRadius(), 1.0f); // Attach a physical circle shape!

    pgn::Vector2 c1Vel = { (float)pgn::Random::Sign(), (float)pgn::Random::Sign() }; 
    float c1Speed = (float)pgn::Random::Roll(5) * 100; 
    c1Body->SetLinearVelocity(c1Vel * c1Speed);

    m_shapes.push_back({ &circle, c1Body });


    // Circle 2
    circle2.setPosition({1000, 500}); 
    circle2.setColor(pgn::Color::Red); 
    circle2.setOutlineThickness(6); 
    
    pgn::PhysicsBody* c2Body = m_world.CreatePhysicsBody(circle2.getPosition(), pgn::PhysicsBodyType::Dynamic); 
    c2Body->AddCircleCollider(circle2.getRadius(), 1.0f); 

    pgn::Vector2 c2Vel = { (float)pgn::Random::Sign(), (float)pgn::Random::Sign() }; 
    float c2Speed = (float)pgn::Random::Roll(5) * 100; 
    c2Body->SetLinearVelocity(c2Vel * c2Speed);

    m_shapes.push_back({ &circle2, c2Body });


    // Standard rendering setup
    sprite.setPosition({400, 200}); 
    sprite.setScale({2,2}); 
    sprite.setZIndex(4); 
    anim.play(pgn::Animation::CreateFromSheet(App().GetTexture("WD"), 48, 48, 0.3f)); 

    line.setColor(pgn::Color::FromHex("#6c1dc7")); 
    line.setPoint1({200,300}); 
    line.setPoint2({100,100}); 
    line.setOutlineThickness(10); 
    line.centerOrigin(); 
    m_areaSystem.LoadLevelMap(std::string(ASSET_PATH) + "maps/Battlefield.tmx"); 
}

void GameLayer::OnEvent(pgn::Event& e)
{
}

void GameLayer::OnUpdate(pgn::Timestep ts)
{
    m_areaSystem.OnUpdate(ts); 

    // 1. Step the physics simulation (this moves the bodies based on their velocities)
    m_world.OnUpdate(ts); 

    float winWidth  = App().GetWindow().GetWindowWidth(); 
    float winHeight = App().GetWindow().GetWindowHeight(); 

    m_camera.setViewportSize(winWidth, winHeight); 

    // ==========================================
    // 3. SYNCHRONIZE VISUALS TO PHYSICS
    // ==========================================
    // Read the exact coordinates calculated by Box2D and apply them to your render elements.
    for (auto& s : m_shapes)
    {
        s.shape->setPosition(s.body->GetPosition());
        s.shape->setRotation(pgn::Math::ToDegrees(s.body->GetRotation()));
    }

    rect.setRotation(rect.getRotation() + 50 * ts.GetSeconds()); 

    if (textsize.isFinished()) 
        textsize = (textsize.getValue()*-1); 

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

    pgn::Vector2 cameraMovement = pgn::Vector2::Zero(); 

    if (pgn::Input::GetKey(pgn::KeyCode::Up))    cameraMovement = pgn::Vector2::Up(); 
    if (pgn::Input::GetKey(pgn::KeyCode::Down))  cameraMovement = pgn::Vector2::Down(); 
    if (pgn::Input::GetKey(pgn::KeyCode::Left))  cameraMovement = pgn::Vector2::Left(); 
    if (pgn::Input::GetKey(pgn::KeyCode::Right)) cameraMovement = pgn::Vector2::Right(); 

    if (cameraMovement.Length() > 0) { cameraMovement.Normalize(); } 

    m_camera.move(cameraMovement * ts.GetSeconds()); 
    m_camera.onUpdate(ts.GetSeconds()); 
}

void GameLayer::OnRender(float alpha)
{
    pgn::Renderer::BeginScene(m_camera.getCamera()); 

    // Render debug shapes overlay (colliders, contact points, etc.)
    m_world.OnRender(alpha); 
    
    pgn::Renderer::Submit(rect); 
    pgn::Renderer::Submit(rect2); 
    pgn::Renderer::Submit(circle); 
    pgn::Renderer::Submit(circle2);
    pgn::Renderer::Submit(circle3); 
    pgn::Renderer::Submit(sprite); 
    pgn::Renderer::Submit(line); 
    pgn::Renderer::Submit(text); 

    pgn::Renderer::EndScene(); 
}