#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsCollider.h"
#include "Physics/PhysicsInternal.h"
#include "Physics/PhysicsLayer.h"
#include "Physics/PhysicsBody.h"
#include "Physics/PhysicsJoint.h"
#include "Events/EngineEvents.h"
#include "Events/WindowEvents.h"
#include "Renderer/Color.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderShape.h"
#include "Renderer/VertexArray.h"

namespace pgn
{
    const float DEBUG_LINE_THICKNESS = 3.f;
    const float DEBUG_LINE_PADDING = 1.f;
    const uint8_t DEBUG_LINE_ALPHA = 180; // 255 is fully opaque, 0 is fully invisible
    const uint8_t DEBUG_BODY_ALPHA = 150;
    const int DEBUG_ZINDEX = MAX_ZINDEX;

    //Helper Methods
    Color ConvertBox2DColor(b2HexColor hexColor, uint8_t alpha = 255)
    {
        uint8_t r = (hexColor >> 16) & 0xFF;
        uint8_t g = (hexColor >> 8) & 0xFF;
        uint8_t b = hexColor & 0xFF;
        return Color{ (float)r, (float)g, (float)b, (float)alpha/255 };
    }

    Transform2D CreateDebugTransform(Vector2 pos, b2WorldTransform transform)
    {
        Transform2D t;
        t.position = pos;
        t.rotation = Math::ToDegrees(b2Rot_GetAngle(transform.q));
        return t;
    }

    template<typename Func>
    void ExecuteWithColliders(b2ShapeId shapeA, b2ShapeId shapeB, Func&& action)
    {
        void* userDataA = b2Shape_GetUserData(shapeA);
        void* userDataB = b2Shape_GetUserData(shapeB);

        if (!userDataA || !userDataB) return;

        auto* colliderA = static_cast<IPhysicsCollider*>(userDataA);
        auto* colliderB = static_cast<IPhysicsCollider*>(userDataB);

        action(colliderA, colliderB);
    }

    b2Rot CalculateReferenceRotation(b2BodyId bodyIdA, b2BodyId bodyIdB)
    {
        float angleA = b2Rot_GetAngle(b2Body_GetRotation(bodyIdA));
        float angleB = b2Rot_GetAngle(b2Body_GetRotation(bodyIdB));
        return b2MakeRot(angleB - angleA);
    }

    //Class Functions
    PhysicsWorld::PhysicsWorld(Vector2 gravity) : ISystem("PhysicsWorld"), m_gravity(gravity)
    {
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = {gravity.x, gravity.y};
        m_worldId = FromB2(b2CreateWorld(&worldDef));

        PhysicsLayer::RegisterLayer("Default");  
    }

    PhysicsWorld::~PhysicsWorld()
    {
        PhysicsLayer::Clear(); 
        m_bodies.clear();
        m_joints.clear();
        if (m_worldId != 0 && b2World_IsValid(ToB2World(m_worldId))) 
            b2DestroyWorld(ToB2World(m_worldId)); 
    }

    PhysicsBody* PhysicsWorld::CreatePhysicsBody(Vector2 pos, PhysicsBodyType type)  
    {
        b2BodyDef bodyDef = b2DefaultBodyDef();
        b2BodyId id = b2CreateBody(ToB2World(m_worldId), &bodyDef);

        auto body = std::unique_ptr<PhysicsBody>(new PhysicsBody(FromB2(id), pos, type));
        PhysicsBody* rawBodyPtr = body.get();
        b2Body_SetUserData(id, rawBodyPtr);
        
        PhysicsBodyCreatedEvent event(rawBodyPtr);
        EventBus::Publish(event);

        m_bodies.push_back(std::move(body));
        return rawBodyPtr;
    }

    void PhysicsWorld::DestroyPhysicsBody(PhysicsBody* body)
    {
        if (!body) return;
        
        auto it = std::find_if(m_bodies.begin(), m_bodies.end(),
            [body](const std::unique_ptr<PhysicsBody>& bp) { 
                return bp.get() == body; 
            });

        if (it != m_bodies.end())
        {
            PhysicsBodyDestroyedEvent event(body);
            EventBus::Publish(event);
            m_bodies.erase(it);
        }
    }

    bool PhysicsWorld::PrepareBaseJointDef(b2JointDef& base, PhysicsBody* bodyA, PhysicsBody* bodyB, Vector2 anchorA, Vector2 anchorB)
    {
        if (!bodyA || !bodyB) return false;

        base.bodyIdA = ToB2Body(bodyA->m_bodyId);
        base.bodyIdB = ToB2Body(bodyB->m_bodyId);
        
        base.localFrameA.p = b2Body_GetLocalPoint(base.bodyIdA, {anchorA.x, anchorA.y});
        base.localFrameB.p = b2Body_GetLocalPoint(base.bodyIdB, {anchorB.x, anchorB.y});
        
        return true;
    }

    DistanceJoint* PhysicsWorld::CreateDistanceJoint(PhysicsBody* bodyA, PhysicsBody* bodyB, Vector2 anchorA, Vector2 anchorB)
    {
        b2DistanceJointDef def = b2DefaultDistanceJointDef();
        if (!PrepareBaseJointDef(def.base, bodyA, bodyB, anchorA, anchorB)) return nullptr;
        
        b2Vec2 pA = b2Body_GetWorldPoint(def.base.bodyIdA, def.base.localFrameA.p);
        b2Vec2 pB = b2Body_GetWorldPoint(def.base.bodyIdB, def.base.localFrameB.p);
        def.length = b2Distance(pA, pB);

        def.base.localFrameA.q = b2Rot_identity;
        def.base.localFrameB.q = b2Rot_identity;

        b2JointId id = b2CreateDistanceJoint(ToB2World(m_worldId), &def);
        return CreateJointPointer<DistanceJoint>(FromB2(id));
    }

    RevoluteJoint* PhysicsWorld::CreateRevoluteJoint(PhysicsBody* bodyA, PhysicsBody* bodyB, Vector2 worldAnchor)
    {
        b2RevoluteJointDef def = b2DefaultRevoluteJointDef();
        if (!PrepareBaseJointDef(def.base, bodyA, bodyB, worldAnchor, worldAnchor)) return nullptr;
        
        def.base.localFrameA.q = b2MakeRot(0.0f); 
        def.base.localFrameB.q = CalculateReferenceRotation(def.base.bodyIdA, def.base.bodyIdB);

        b2JointId id = b2CreateRevoluteJoint(ToB2World(m_worldId), &def);
        return CreateJointPointer<RevoluteJoint>(FromB2(id));
    }

    PrismaticJoint* PhysicsWorld::CreatePrismaticJoint(PhysicsBody* bodyA, PhysicsBody* bodyB, Vector2 worldAnchor, Vector2 worldAxis)
    {
        b2PrismaticJointDef def = b2DefaultPrismaticJointDef();
        if (!PrepareBaseJointDef(def.base, bodyA, bodyB, worldAnchor, worldAnchor)) return nullptr;
        
        b2Vec2 localAxisA = b2Body_GetLocalVector(def.base.bodyIdA, {worldAxis.x, worldAxis.y});
        def.base.localFrameA.q = b2ComputeRotationBetweenUnitVectors({1.0f, 0.0f}, localAxisA);
        def.base.localFrameB.q = CalculateReferenceRotation(def.base.bodyIdA, def.base.bodyIdB);

        b2JointId id = b2CreatePrismaticJoint(ToB2World(m_worldId), &def);
        return CreateJointPointer<PrismaticJoint>(FromB2(id));
    }

    void PhysicsWorld::DestroyJoint(PhysicsJoint* joint)
    {
        if (!joint) return;
        b2DestroyJoint(ToB2Joint(joint->m_jointId), true);

        auto it = std::find_if(m_joints.begin(), m_joints.end(),
            [joint](const std::unique_ptr<PhysicsJoint>& wrapped) {
                return wrapped.get() == joint;
            });

        if (it != m_joints.end())
        {
            m_joints.erase(it);
        }
    }

    void pgn::PhysicsWorld::DrawDebug()
    {
        if (!m_isDebugOn) return;
        if (m_worldId == 0 || !b2World_IsValid(ToB2World(m_worldId))) return;

        auto ExecutePass = [this](bool showJoints, bool showContacts, auto passFunction) {
            b2DebugDraw dd = b2DefaultDebugDraw();
            dd.drawShapes = true;
            dd.drawJoints = showJoints;
            dd.drawContacts = showContacts;
            dd.context = this;
            passFunction(dd);
            b2World_Draw(ToB2World(m_worldId), &dd);
        };

        if (m_drawDebugBodies)    ExecutePass(false, false, [this](b2DebugDraw& dd) { DrawBodies(dd); });
        if (m_drawDebugColliders) ExecutePass(true,  true,  [this](b2DebugDraw& dd) { DrawColliders(dd); });
    }

    void PhysicsWorld::DrawBodies(b2DebugDraw& debugDraw)
    {
        // Circle Bodies
        debugDraw.DrawSolidCircleFcn = [](b2WorldTransform transform, b2Vec2 center, float radius, b2HexColor color, void* context) {
            b2Vec2 worldCenter = b2RotateVector(transform.q, center) + transform.p;
            CircleShape circle;
            circle.setRadius(radius);
            circle.setColor(ConvertBox2DColor(color, DEBUG_BODY_ALPHA));
            circle.setTransform(CreateDebugTransform({ worldCenter.x, worldCenter.y }, transform));
            circle.centerOrigin();
            circle.setZIndex(DEBUG_ZINDEX);
            Renderer::Submit(circle);
        };

        // Polygon Bodies (Boxes & Triangles)
        debugDraw.DrawSolidPolygonFcn = [](b2WorldTransform transform, const b2Vec2* vertices, int32_t vertexCount, float radius, b2HexColor color, void* context) {
            if (vertexCount < 3) return;
            else if (vertexCount == 4)
            {
                float width  = std::abs(vertices[1].x - vertices[0].x);
                float height = std::abs(vertices[2].y - vertices[1].y);

                RectangleShape rect;
                rect.setSize({ width, height });
                rect.setColor(ConvertBox2DColor(color, DEBUG_BODY_ALPHA));
                rect.setTransform(CreateDebugTransform({ transform.p.x, transform.p.y }, transform));
                rect.centerOrigin();
                rect.setZIndex(DEBUG_ZINDEX);
                Renderer::Submit(rect);
            }
            else
            {
                VertexArray va;
                Color fillCol = ConvertBox2DColor(color, DEBUG_BODY_ALPHA);
                for (int32_t i = 0; i < vertexCount; ++i) { va.addVertex({ {vertices[i].x, vertices[i].y}, fillCol, {0.0f, 0.0f} }); }
                for (int32_t i = 1; i < vertexCount - 1; ++i) { va.addIndex(0); va.addIndex(i); va.addIndex(i + 1); }
                Renderer::Submit(va, nullptr, CreateDebugTransform({ transform.p.x, transform.p.y }, transform), DEBUG_ZINDEX);
            }
        };

        // Capsule Bodies
        debugDraw.DrawSolidCapsuleFcn = [](b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context) {
            CircleShape cap1, cap2;
            cap1.setRadius(radius); cap1.setColor(ConvertBox2DColor(color, DEBUG_BODY_ALPHA));
            cap2.setRadius(radius); cap2.setColor(ConvertBox2DColor(color, DEBUG_BODY_ALPHA));
            
            Transform2D t1, t2;
            t1.position = {p1.x, p1.y}; cap1.setTransform(t1);
            t2.position = {p2.x, p2.y}; cap2.setTransform(t2);

            cap1.setZIndex(DEBUG_ZINDEX);
            cap1.setZIndex(DEBUG_ZINDEX);

            Renderer::Submit(cap1);
            Renderer::Submit(cap2);
        };
    }

    void PhysicsWorld::DrawColliders(b2DebugDraw& debugDraw)
    {
        // Base Line Drawing (Joints, Grid grids, Raycasts)
        debugDraw.DrawLineFcn = [](b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* context) {
            LineShape line;
            line.setPoints({p1.x, p1.y}, {p2.x, p2.y});
            line.setColor(ConvertBox2DColor(color, DEBUG_LINE_ALPHA));
            line.setOutlineThickness(DEBUG_LINE_THICKNESS);
            line.setZIndex(DEBUG_ZINDEX);
            Renderer::Submit(line);
        };

        // Circle Colliders (Expanded Ring)
        debugDraw.DrawSolidCircleFcn = [](b2WorldTransform transform, b2Vec2 center, float radius, b2HexColor color, void* context) {
            b2Vec2 worldCenter = b2RotateVector(transform.q, center) + transform.p;
            CircleShape colliderOutline;
            colliderOutline.setRadius(radius + DEBUG_LINE_PADDING);
            colliderOutline.setColor(ConvertBox2DColor(color, DEBUG_LINE_ALPHA));
            colliderOutline.setOutlineThickness(DEBUG_LINE_THICKNESS);
            colliderOutline.setTransform(CreateDebugTransform({ worldCenter.x, worldCenter.y }, transform));
            colliderOutline.centerOrigin();
            colliderOutline.setZIndex(DEBUG_ZINDEX);
            Renderer::Submit(colliderOutline);
        };

        // Polygon Colliders (Expanded Rectangles / Outlined Hulls)
        debugDraw.DrawSolidPolygonFcn = [](b2WorldTransform transform, const b2Vec2* vertices, int32_t vertexCount, float radius, b2HexColor color, void* context) {
            if (vertexCount < 3) return;

            if (vertexCount == 4)
            {
                float width  = std::abs(vertices[1].x - vertices[0].x);
                float height = std::abs(vertices[2].y - vertices[1].y);

                RectangleShape colliderOutline;
                colliderOutline.setSize({ width + (DEBUG_LINE_PADDING * 2.f), height + (DEBUG_LINE_PADDING * 2.f) });
                colliderOutline.setColor(ConvertBox2DColor(color, DEBUG_LINE_ALPHA));
                colliderOutline.setOutlineThickness(DEBUG_LINE_THICKNESS);
                colliderOutline.setTransform(CreateDebugTransform({ transform.p.x, transform.p.y }, transform));
                colliderOutline.centerOrigin();
                colliderOutline.setZIndex(DEBUG_ZINDEX);
                Renderer::Submit(colliderOutline);
            }
            else
            {
                for (int32_t i = 0; i < vertexCount; ++i)
                {
                    b2Vec2 localP1 = vertices[i];
                    b2Vec2 localP2 = vertices[(i + 1) % vertexCount];
                    b2Vec2 wP1 = b2RotateVector(transform.q, localP1) + transform.p;
                    b2Vec2 wP2 = b2RotateVector(transform.q, localP2) + transform.p;

                    LineShape border;
                    border.setPoints({wP1.x, wP1.y}, {wP2.x, wP2.y});
                    border.setColor(ConvertBox2DColor(color, DEBUG_LINE_ALPHA));
                    border.setOutlineThickness(DEBUG_LINE_THICKNESS);
                    border.setZIndex(DEBUG_ZINDEX);
                    Renderer::Submit(border);
                }
            }
        };

        // Capsule Colliders
        debugDraw.DrawSolidCapsuleFcn = [](b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color, void* context) {
            LineShape core;
            core.setPoints({p1.x, p1.y}, {p2.x, p2.y});
            core.setColor(ConvertBox2DColor(color, DEBUG_LINE_ALPHA));
            core.setOutlineThickness((radius * 2.0f) + DEBUG_LINE_THICKNESS);
            core.setZIndex(DEBUG_ZINDEX);
            Renderer::Submit(core);
        };
    }

    template<typename TEvent>
    void PhysicsWorld::ProcessContact(b2ShapeId& shapeA, b2ShapeId& shapeB, bool isBeginPhase)
    {
        ExecuteWithColliders(shapeA, shapeB, [&](IPhysicsCollider* colA, IPhysicsCollider* colB) {
            if (isBeginPhase)
            {
                colA->InvokeOnContactBegin(colB);
                colB->InvokeOnContactBegin(colA);
            }
            else
            {
                colA->InvokeOnContactEnd(colB);
                colB->InvokeOnContactEnd(colA);
            }

            TEvent contactEvent(colA, colB, isBeginPhase);
            App().RaiseEvent(contactEvent);
        });
    }

    void PhysicsWorld::ProcessContactHit(b2ShapeId& shapeA, b2ShapeId& shapeB, b2Vec2& point, b2Vec2& normal, float speed)
    {
        ExecuteWithColliders(shapeA, shapeB, [&](IPhysicsCollider* colA, IPhysicsCollider* colB) {
            ColliderHitEvent hitEvent(colA, colB, { point.x, point.y }, { normal.x, normal.y }, speed);
            App().RaiseEvent(hitEvent);
        });
    }

    void PhysicsWorld::OnEvent(Event &e)
    {
        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<WindowMinimizedEvent>([this](WindowMinimizedEvent& e) {
			m_isPaused = true;
			return false; 
		});

		dispatcher.Dispatch<WindowRestoredEvent>([this](WindowRestoredEvent& e) {
			m_isPaused = false;
			return false;
		});
    }

    void PhysicsWorld::OnUpdate(Timestep ts)
    {
        if(m_isPaused) return;

        float dt = ts.GetSeconds();
        
        b2World_Step(ToB2World(m_worldId), dt, m_subStepCount);

        b2SensorEvents sensorEvents = b2World_GetSensorEvents(ToB2World(m_worldId));
        for (int i = 0; i < sensorEvents.beginCount; ++i)
        {
            b2SensorBeginTouchEvent event = sensorEvents.beginEvents[i];
            ProcessContact<ColliderTriggerEvent>(event.sensorShapeId, event.visitorShapeId, true);
        }
        for (int i = 0; i < sensorEvents.endCount; ++i)
        {
            b2SensorEndTouchEvent event = sensorEvents.endEvents[i];
            ProcessContact<ColliderTriggerEvent>(event.sensorShapeId, event.visitorShapeId, false);
        }

        b2ContactEvents contactEvents = b2World_GetContactEvents(ToB2World(m_worldId));
        for (int i = 0; i < contactEvents.beginCount; ++i)
        {
            b2ContactBeginTouchEvent event = contactEvents.beginEvents[i];
            ProcessContact<ColliderContactEvent>(event.shapeIdA, event.shapeIdB, true);
        }
        for (int i = 0; i < contactEvents.endCount; ++i)
        {
            b2ContactEndTouchEvent event = contactEvents.endEvents[i];
            ProcessContact<ColliderContactEvent>(event.shapeIdA, event.shapeIdB, false);
        }

        for (int i = 0; i < contactEvents.hitCount; ++i)
        {
            b2ContactHitEvent event = contactEvents.hitEvents[i];
            ProcessContactHit(event.shapeIdA, event.shapeIdB, event.point, event.normal, event.approachSpeed);
        }

        b2BodyEvents bodyEvents = b2World_GetBodyEvents(ToB2World(m_worldId));
        for (int i = 0; i < bodyEvents.moveCount; ++i)
        {
            b2BodyMoveEvent event = bodyEvents.moveEvents[i];
            auto* bodyWrapper = static_cast<PhysicsBody*>(b2Body_GetUserData(event.bodyId));
            
            if (bodyWrapper)
            {
                PhysicsBodyMoveEvent moveEvent(bodyWrapper, { event.transform.p.x, event.transform.p.y }, b2Rot_GetAngle(event.transform.q));
                App().RaiseEvent(moveEvent);
            }
        }
    }

    void PhysicsWorld::OnRender(float alpha)
    {
        DrawDebug(); 
    }
} // namespace pgn