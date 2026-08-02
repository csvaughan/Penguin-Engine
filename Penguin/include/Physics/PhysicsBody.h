#pragma once
#include "Math/Vector.h"

namespace pgn
{
    class BoxCollider;
    class PolygonCollider;
    class CircleCollider;
    class IPhysicsCollider;

    enum class PhysicsBodyType { Static, Kinematic, Dynamic };

    class PhysicsBody
    {
    public:
        
        ~PhysicsBody();

        // --- Colliders ---
        BoxCollider* AddBoxCollider(Vector2 extents, float density, bool isTrigger = false);
        CircleCollider* AddCircleCollider(float radius, float density, bool isTrigger = false);
        PolygonCollider* AddPolygonCollider(const std::vector<Vector2>& points, float density, bool isTrigger = false);
        void RemoveCollider(IPhysicsCollider* collider);
        void RemoveAllColliders();

        // --- Core Transformations ---
        Vector2 GetPosition() const;
        float GetRotation() const; 
        void SetTransform(Vector2 position, float angleInRadians);

        // --- Motion & Forces ---
        Vector2 GetLinearVelocity() const;
        void SetLinearVelocity(Vector2 velocity);
        float GetAngularVelocity() const;
        void SetAngularVelocity(float omega);

        void ApplyForce(Vector2 force, Vector2 worldPoint, bool wake = true);
        void ApplyForceToCenter(Vector2 force, bool wake = true);
        void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPoint, bool wake = true);
        void ApplyLinearImpulseToCenter(Vector2 impulse, bool wake = true);
        void ApplyTorque(float torque, bool wake = true);

        // --- Configuration Parameters ---
        void SetType(PhysicsBodyType type);
        PhysicsBodyType GetType() const;
        void SetGravityScale(float scale);
        float GetGravityScale() const;
        void SetLinearDamping(float damping);
        float GetLinearDamping() const;
        void SetAngularDamping(float damping);
        float GetAngularDamping() const;

        // --- Simulation State ---
        void SetAwake(bool awake);
        bool IsAwake() const;
        void SetEnabled(bool enabled);
        bool IsEnabled() const;
        void SetBullet(bool flag);
        bool IsBullet() const;
        float GetMass() const;

    private:
        
        template <typename TCollider, typename TB2Shape, typename CreatorFunc, typename ConfigFunc>
        TCollider* CreateAndAttachShape(float density, bool isTrigger, TB2Shape& b2Shape, CreatorFunc creator, ConfigFunc config);

        PhysicsBody(uint64_t bodyId, Vector2 pos, PhysicsBodyType type);
        
        uint64_t m_bodyId = 0;
        std::vector<std::unique_ptr<IPhysicsCollider>> m_colliders;

        friend class PhysicsWorld;
    };
} // namespace pgn