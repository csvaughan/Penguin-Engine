#include "Physics/PhysicsBody.h"
#include "Physics/PhysicsCollider.h"
#include "Physics/PhysicsLayer.h"
#include "Physics/PhysicsInternal.h"

namespace pgn
{
    b2ShapeDef CreateShapeDef(IPhysicsCollider* ptr, float density, bool isTrigger)
    {
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = density;
        shapeDef.isSensor = isTrigger;
        shapeDef.userData = ptr;
        shapeDef.filter.categoryBits = PhysicsLayer::GetLayerMask("Default"); 
        shapeDef.filter.maskBits = ~0ULL; 
        return shapeDef;
    }
    
    PhysicsBody::PhysicsBody(uint64_t bodyId, Vector2 pos ,PhysicsBodyType type) : m_bodyId(bodyId) 
    {
        SetType(type);
        SetTransform(pos, GetRotation());
    }

    PhysicsBody::~PhysicsBody()
    {
        RemoveAllColliders();
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2DestroyBody(ToB2Body(m_bodyId));
    }

    template <typename TCollider, typename TB2Shape, typename CreatorFunc, typename ConfigFunc>
    TCollider* PhysicsBody::CreateAndAttachShape(float density, bool isTrigger, TB2Shape& b2Shape, CreatorFunc creator, ConfigFunc config)
    {
        auto collider = std::unique_ptr<TCollider>(new TCollider(isTrigger));
        auto ptr = collider.get();
        m_colliders.push_back(std::move(collider));

        b2ShapeDef shapeDef = CreateShapeDef(ptr, density, isTrigger);
        config(ptr);
        ptr->m_shapeId = FromB2(creator(ToB2Body(m_bodyId), &shapeDef, &b2Shape));
        return ptr;
    }

    BoxCollider* PhysicsBody::AddBoxCollider(Vector2 extents, float density, bool isTrigger)
    {
        b2Polygon box = b2MakeBox(extents.x, extents.y);
        return CreateAndAttachShape<BoxCollider>(density, isTrigger, box, b2CreatePolygonShape,
            [&extents](BoxCollider* ptr) {
                ptr->SetSize({ extents.x * 2.0f, extents.y * 2.0f });
            });
    }

    CircleCollider* PhysicsBody::AddCircleCollider(float radius, float density, bool isTrigger)
    {
        b2Circle circle = { {0.0f, 0.0f}, radius };
        return CreateAndAttachShape<CircleCollider>(density, isTrigger, circle, b2CreateCircleShape,
            [radius](CircleCollider* ptr) {
                ptr->SetRadius(radius);
            });
    }

    PolygonCollider* PhysicsBody::AddPolygonCollider(const std::vector<Vector2>& points, float density, bool isTrigger)
    {
        if (points.size() < 3) return nullptr;

        std::vector<b2Vec2> b2Points;
        b2Points.reserve(points.size());
        for (const auto& p : points)
            b2Points.push_back({ p.x, p.y });

        // Compute the convex hull from the raw point cloud
        b2Hull hull = b2ComputeHull(b2Points.data(), static_cast<int32_t>(b2Points.size()));
        if (hull.count == 0) return nullptr; // Failed (collinear/degenerate)

        b2Polygon polygon = b2MakePolygon(&hull, 0.0f);

        return CreateAndAttachShape<PolygonCollider>(density, isTrigger, polygon, b2CreatePolygonShape, 
            [](PolygonCollider* ptr) {});
    }

    void pgn::PhysicsBody::RemoveCollider(IPhysicsCollider* collider)
    {
        if(!collider) return;

        m_colliders.erase(
            std::remove_if(m_colliders.begin(), m_colliders.end(),
                [collider](const std::unique_ptr<IPhysicsCollider>& wrapped) {
                    return wrapped.get() == collider;
            }),
        m_colliders.end());
    }

    void PhysicsBody::RemoveAllColliders()
    {
        m_colliders.clear();
    }

    Vector2 PhysicsBody::GetPosition() const
    {
        b2Vec2 pos = b2Body_GetPosition(ToB2Body(m_bodyId));
        return { pos.x, pos.y };
    }

    float PhysicsBody::GetRotation() const
    {
        return b2Rot_GetAngle(b2Body_GetRotation(ToB2Body(m_bodyId)));
    }

    void PhysicsBody::SetTransform(Vector2 position, float angleInRadians)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_SetTransform(ToB2Body(m_bodyId), { position.x, position.y }, b2MakeRot(angleInRadians));
    }

    Vector2 PhysicsBody::GetLinearVelocity() const
    {
        b2Vec2 vel = b2Body_GetLinearVelocity(ToB2Body(m_bodyId));
        return { vel.x, vel.y };
    }

    void PhysicsBody::SetLinearVelocity(Vector2 velocity)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_SetLinearVelocity(ToB2Body(m_bodyId), { velocity.x, velocity.y });
    }

    float PhysicsBody::GetAngularVelocity() const
    {
        return b2Body_GetAngularVelocity(ToB2Body(m_bodyId));
    }

    void PhysicsBody::SetAngularVelocity(float omega)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_SetAngularVelocity(ToB2Body(m_bodyId), omega);
    }

    void PhysicsBody::ApplyForce(Vector2 force, Vector2 worldPoint, bool wake)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_ApplyForce(ToB2Body(m_bodyId), { force.x, force.y }, { worldPoint.x, worldPoint.y }, wake);
    }

    void PhysicsBody::ApplyForceToCenter(Vector2 force, bool wake)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_ApplyForceToCenter(ToB2Body(m_bodyId), { force.x, force.y }, wake);
    }

    void PhysicsBody::ApplyLinearImpulse(Vector2 impulse, Vector2 worldPoint, bool wake)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_ApplyLinearImpulse(ToB2Body(m_bodyId), { impulse.x, impulse.y }, { worldPoint.x, worldPoint.y }, wake);
    }

    void PhysicsBody::ApplyLinearImpulseToCenter(Vector2 impulse, bool wake)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_ApplyLinearImpulseToCenter(ToB2Body(m_bodyId), { impulse.x, impulse.y }, wake);
    }

    void PhysicsBody::ApplyTorque(float torque, bool wake)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_ApplyTorque(ToB2Body(m_bodyId), torque, wake);
    }

    void PhysicsBody::SetType(PhysicsBodyType type)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
        {    
            b2BodyType b2type; 
            switch (type)
            {
                case PhysicsBodyType::Static:    b2type = b2_staticBody; break;
                case PhysicsBodyType::Kinematic: b2type = b2_kinematicBody; break;
                case PhysicsBodyType::Dynamic:   b2type = b2_dynamicBody; break;
            }
            b2Body_SetType(ToB2Body(m_bodyId), b2type);
        }
    }

    PhysicsBodyType PhysicsBody::GetType() const
    {
        switch (b2Body_GetType(ToB2Body(m_bodyId)))
        {
            case b2_staticBody:    return PhysicsBodyType::Static;
            case b2_kinematicBody: return PhysicsBodyType::Kinematic;
            case b2_dynamicBody:   return PhysicsBodyType::Dynamic;
            default:               return PhysicsBodyType::Static;
        } 
    }

    void PhysicsBody::SetGravityScale(float scale)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_SetGravityScale(ToB2Body(m_bodyId), scale);
    }

    float PhysicsBody::GetGravityScale() const
    {
        return b2Body_GetGravityScale(ToB2Body(m_bodyId));
    }

    void PhysicsBody::SetLinearDamping(float damping)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_SetLinearDamping(ToB2Body(m_bodyId), damping);
    }

    float PhysicsBody::GetLinearDamping() const
    {
        return b2Body_GetLinearDamping(ToB2Body(m_bodyId));
    }

    void PhysicsBody::SetAngularDamping(float damping)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_SetAngularDamping(ToB2Body(m_bodyId), damping);
    }

    float PhysicsBody::GetAngularDamping() const
    {
        return b2Body_GetAngularDamping(ToB2Body(m_bodyId));
    }

    void PhysicsBody::SetAwake(bool awake)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_SetAwake(ToB2Body(m_bodyId), awake);
    }

    bool PhysicsBody::IsAwake() const
    {
        return b2Body_IsAwake(ToB2Body(m_bodyId));
    }

    void PhysicsBody::SetEnabled(bool enabled)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId)))
        {
            if (enabled) b2Body_Enable(ToB2Body(m_bodyId));
            else b2Body_Disable(ToB2Body(m_bodyId));
        }
    }

    bool PhysicsBody::IsEnabled() const
    {
        return b2Body_IsEnabled(ToB2Body(m_bodyId));
    }

    void PhysicsBody::SetBullet(bool flag)
    {
        if (m_bodyId != 0 && b2Body_IsValid(ToB2Body(m_bodyId))) 
            b2Body_SetBullet(ToB2Body(m_bodyId), flag);
    }

    bool PhysicsBody::IsBullet() const
    {
        return b2Body_IsBullet(ToB2Body(m_bodyId));
    }

    float PhysicsBody::GetMass() const
    {
        return b2Body_GetMass(ToB2Body(m_bodyId));
    }
} // namespace pgn