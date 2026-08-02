#pragma once
#include "Event.h"
#include "Core/State.h"
#include "Core/Layer.h"
#include "Memory/Scope.h"

namespace pgn
{
    class StateChangeEvent : public Event
    {
    public:
        StateChangeEvent(Scope<State> newState) : m_NewState(std::move(newState)) {}

        Scope<State> GetState() { return std::move(m_NewState); }

        EVENT_CLASS_TYPE(StateChange)
        EVENT_CLASS_CATEGORY(EventCategoryEngine)
    private:
        Scope<State> m_NewState;
    };

    class AppQuitEvent : public Event
    {
    public:
        enum class QuitReason { UserExit, Error, Restart };

        AppQuitEvent(QuitReason reason = QuitReason::UserExit) : m_Reason(reason) {}
        
        QuitReason GetReason() const { return m_Reason; }
        
        EVENT_CLASS_TYPE(AppQuit)
        EVENT_CLASS_CATEGORY(EventCategoryEngine)
    private:
        QuitReason m_Reason;
    };

    class LayerPushEvent : public Event
    {
    public:
        LayerPushEvent(Ref<Layer> layer) : m_layer(layer){}
        Ref<Layer> GetLayer() { return m_layer; }
        EVENT_CLASS_TYPE(LayerPush);
        EVENT_CLASS_CATEGORY(EventCategoryEngine)
    private:
        Ref<Layer> m_layer;
    };

    class LayerPopEvent : public Event
    {
    public:
        LayerPopEvent(Ref<Layer> layer) : m_layer(layer){}
        Ref<Layer> GetLayer() { return m_layer; }
        EVENT_CLASS_TYPE(LayerPop);
        EVENT_CLASS_CATEGORY(EventCategoryEngine)
    private:
        Ref<Layer> m_layer;
    };

    // ========================================================================
    // Physics Events
    // ========================================================================

    class PhysicsBody;
    class IPhysicsCollider;

    class PhysicsBodyCreatedEvent : public Event
    {
    public:
        PhysicsBodyCreatedEvent(PhysicsBody* body) : m_body(body) {}

        PhysicsBody* GetBody() const { return m_body; }

        EVENT_CLASS_TYPE(PhysicsBodyCreated)
        EVENT_CLASS_CATEGORY(EventCategoryEngine)
    private:
        PhysicsBody* m_body;
    };

    class PhysicsBodyDestroyedEvent : public Event
    {
    public:
        PhysicsBodyDestroyedEvent(PhysicsBody* body) : m_body(body) {}

        PhysicsBody* GetBody() const { return m_body; }

        EVENT_CLASS_TYPE(PhysicsBodyDestroyed)
        EVENT_CLASS_CATEGORY(EventCategoryEngine)
    private:
        PhysicsBody* m_body;
    };

    // ========================================================================
    // Collider-Related Events
    // ========================================================================
    class ColliderEvent : public Event
    {
    public:
        ColliderEvent(IPhysicsCollider* a, IPhysicsCollider* b)
            : m_colliderA(a), m_colliderB(b) {}

        IPhysicsCollider* GetColliderA() const { return m_colliderA; }
        IPhysicsCollider* GetColliderB() const { return m_colliderB; }

        // Helper helper to quickly check if a specific collider is involved
        bool HasCollider(IPhysicsCollider* collider) const {
            return m_colliderA == collider || m_colliderB == collider;
        }

    protected:
        IPhysicsCollider* m_colliderA;
        IPhysicsCollider* m_colliderB;
    };

    // ========================================================================
    // DERIVED EVENTS
    // ========================================================================

    class ColliderContactEvent : public ColliderEvent
    {
    public:
        ColliderContactEvent(IPhysicsCollider* a, IPhysicsCollider* b, bool begin)
            : ColliderEvent(a, b), m_isBeginPhase(begin) {}

        bool IsBeginPhase() const { return m_isBeginPhase; }

        EVENT_CLASS_TYPE(ColliderContact)
        EVENT_CLASS_CATEGORY(EventCategoryEngine)

    private:
        bool m_isBeginPhase;
    };

    class ColliderTriggerEvent : public ColliderEvent
    {
    public:
        ColliderTriggerEvent(IPhysicsCollider* trigger, IPhysicsCollider* visitor, bool begin)
            : ColliderEvent(trigger, visitor), m_isBeginPhase(begin) {}

        // Semantic aliases that wrap the base class accessors for code clarity
        IPhysicsCollider* GetTrigger() const { return m_colliderA; }
        IPhysicsCollider* GetVisitor() const { return m_colliderB; }
        bool IsBeginPhase() const { return m_isBeginPhase; }

        EVENT_CLASS_TYPE(ColliderTrigger)
        EVENT_CLASS_CATEGORY(EventCategoryEngine)

    private:
        bool m_isBeginPhase;
    };

    class ColliderHitEvent : public ColliderEvent
    {
    public:
        ColliderHitEvent(IPhysicsCollider* a, IPhysicsCollider* b, Vector2 point, Vector2 normal, float speed)
            : ColliderEvent(a, b), m_point(point), m_normal(normal), m_approachSpeed(speed) {}

        Vector2 GetHitPoint() const { return m_point; }
        Vector2 GetHitNormal() const { return m_normal; }
        float GetApproachSpeed() const { return m_approachSpeed; }

        EVENT_CLASS_TYPE(ColliderHit)
        EVENT_CLASS_CATEGORY(EventCategoryEngine)

    private:
        Vector2 m_point;
        Vector2 m_normal;
        float m_approachSpeed;
    };

    class PhysicsBodyMoveEvent : public Event
    {
    public:
        PhysicsBodyMoveEvent(PhysicsBody* body, Vector2 position, float rotation)
            : m_body(body), m_position(position), m_rotation(rotation) {}

        PhysicsBody* GetBody() const { return m_body; }
        Vector2 GetPosition() const { return m_position; }
        float GetRotation() const { return m_rotation; }

        EVENT_CLASS_TYPE(PhysicsBodyMove)
        EVENT_CLASS_CATEGORY(EventCategoryEngine)

    private:
        PhysicsBody* m_body;
        Vector2 m_position;
        float m_rotation;
    };
} // namespace pgn