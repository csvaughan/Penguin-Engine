#include "Physics/PhysicsCollider.h"
#include "Physics/PhysicsLayer.h"
#include "Physics/PhysicsInternal.h"

namespace pgn
{
    IPhysicsCollider::IPhysicsCollider(bool isTrigger) : m_isTrigger(isTrigger) {}
    IPhysicsCollider::~IPhysicsCollider()
    {
        b2ShapeId shapeId = ToB2Shape(m_shapeId);
        if (b2Shape_IsValid(shapeId))
            b2DestroyShape(shapeId, false);
    }

    void IPhysicsCollider::SetDensity(float density)
    {
        if (m_shapeId != 0 && b2Shape_IsValid(ToB2Shape(m_shapeId)))
            b2Shape_SetDensity(ToB2Shape(m_shapeId), density, true);
    }

    float IPhysicsCollider::GetDensity() const
    {
        if (m_shapeId != 0 && b2Shape_IsValid(ToB2Shape(m_shapeId)))
            return b2Shape_GetDensity(ToB2Shape(m_shapeId));
        return 0.0f;
    }

    void IPhysicsCollider::SetLayer(const std::string& layerName)
    {
        if (m_shapeId == 0 || !b2Shape_IsValid(ToB2Shape(m_shapeId))) return;
        
        b2Filter filter = b2Shape_GetFilter(ToB2Shape(m_shapeId));
        
        filter.categoryBits = PhysicsLayer::GetLayerMask(layerName); 
        filter.maskBits = PhysicsLayer::GetCollisionMask(layerName); 
        
        b2Shape_SetFilter(ToB2Shape(m_shapeId), filter);

        // Wake up the parent body so Box2D re-evaluates overlapping shapes immediately
        b2BodyId bodyId = b2Shape_GetBody(ToB2Shape(m_shapeId));
        if (b2Body_IsValid(bodyId))
        {
            b2Body_SetAwake(bodyId, true);
        }
    }

    //Shape Functions
    void BoxCollider::SetSize(Vector2 size) 
    { 
        m_size = size;
        if (m_shapeId == 0 || !b2Shape_IsValid(ToB2Shape(m_shapeId))) return;

        b2Polygon box = b2MakeBox(m_size.x * 0.5f, m_size.y * 0.5f);
        b2Shape_SetPolygon(ToB2Shape(m_shapeId), &box);
    }

    void CircleCollider::SetRadius(float radius) 
    {
        m_radius = radius; 
        if (m_shapeId == 0 || !b2Shape_IsValid(ToB2Shape(m_shapeId))) return;

        b2Circle circle = b2Shape_GetCircle(ToB2Shape(m_shapeId));
        circle.radius = m_radius;
        b2Shape_SetCircle(ToB2Shape(m_shapeId), &circle);
    }
} // namespace pgn