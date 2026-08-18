#pragma once
#include "Core/System.h"

struct b2DebugDraw;
struct b2JointDef;
struct b2ShapeId;
struct b2Vec2;

namespace pgn
{
    class PhysicsBody;
    class PhysicsJoint;
    class DistanceJoint;
    class RevoluteJoint;
    class PrismaticJoint;
    enum class PhysicsBodyType;

    class PhysicsWorld : public BaseSystem
    {
    public:
        PhysicsWorld(Vector2 gravity);
        ~PhysicsWorld();

        PhysicsBody* CreatePhysicsBody(Vector2 pos, PhysicsBodyType type);
        void DestroyPhysicsBody(PhysicsBody* body);

        DistanceJoint* CreateDistanceJoint(PhysicsBody* bodyA, PhysicsBody* bodyB, Vector2 anchorA, Vector2 anchorB);
        RevoluteJoint* CreateRevoluteJoint(PhysicsBody* bodyA, PhysicsBody* bodyB, Vector2 worldAnchor);
        PrismaticJoint* CreatePrismaticJoint(PhysicsBody* bodyA, PhysicsBody* bodyB, Vector2 worldAnchor, Vector2 worldAxis);
        void DestroyJoint(PhysicsJoint* joint);

        void SetPaused(bool paused) { m_isPaused = paused; }
        bool IsPaused() { return m_isPaused; }

        void OnEvent(Event& e) override;
        void OnUpdate(Timestep ts) override;
        void OnRender(float alpha, Renderer& renderer) override;

        void EnableDebugMode(bool enabled) { m_isDebugOn = enabled; }
        bool IsDebugEnabled() const { return m_isDebugOn; }
        void EnableDrawDebugBodies(bool draw) { m_drawDebugBodies = draw; }
        void EnableDrawDebugColliders(bool draw) { m_drawDebugColliders = draw; }

        bool IsDebugBodiesEnabled() const { return m_drawDebugBodies; }
        bool IsDebugCollidersEnabled() const { return m_drawDebugColliders; }

        void SetSubStepCount(int32_t count) { m_subStepCount = count < 1 ? 1 : count; }
        int32_t GetSubStepCount() const { return m_subStepCount; }
        
    private:
        using BaseSystem::OnEnter;
        using BaseSystem::OnExit; 

        template<typename TEvent>
        void ProcessContact(b2ShapeId& shapeA, b2ShapeId& shapeB, bool isBeginPhase);
        void ProcessContactHit(b2ShapeId& shapeA, b2ShapeId& shapeB, b2Vec2& point, b2Vec2& normal, float speed);

        template<typename TJoint>
        TJoint* CreateJointPointer(uint64_t id)
        {
            auto joint = std::unique_ptr<TJoint>(new TJoint(id));
            TJoint* ptr = joint.get();
            m_joints.push_back(std::move(joint));
            return ptr;
        }

        bool PrepareBaseJointDef(b2JointDef& base, PhysicsBody* bodyA, PhysicsBody* bodyB, Vector2 anchorA, Vector2 anchorB);
        
        void DrawDebug(Renderer& renderer);
        void DrawBodies(b2DebugDraw& debugDraw, Renderer& renderer);  
        void DrawColliders(b2DebugDraw& debugDraw, Renderer& renderer);

        bool m_isPaused = false;
        bool m_isDebugOn = true;
        bool m_drawDebugBodies = true;   
        bool m_drawDebugColliders = true; 

        Vector2 m_gravity;
        uint64_t m_worldId = 0;
        int32_t m_subStepCount = 4;
        std::vector<std::unique_ptr<PhysicsBody>> m_bodies;
        std::vector<std::unique_ptr<PhysicsJoint>> m_joints;
    };
} // namespace pgn