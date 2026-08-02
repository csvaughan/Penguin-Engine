#pragma once
#include "Math/Vector.h"

namespace pgn
{
    class IPhysicsCollider
    {
    public:
        virtual ~IPhysicsCollider();

        void SetOnContactBegin(std::function<void(IPhysicsCollider* other)> cb) { m_onContactBegin = cb; }
        void SetOnContactEnd(std::function<void(IPhysicsCollider* other)> cb)   { m_onContactEnd = cb; }

        void InvokeOnContactBegin(IPhysicsCollider* other) { if (m_onContactBegin) m_onContactBegin(other); }
        void InvokeOnContactEnd(IPhysicsCollider* other)   { if (m_onContactEnd) m_onContactEnd(other); }

        bool IsTrigger() const { return m_isTrigger; }

        void SetDensity(float density);
        float GetDensity() const;

        void SetLayer(const std::string& layerName);

    protected:
        IPhysicsCollider(bool isTrigger);
        uint64_t m_shapeId = 0;
        bool m_isTrigger;
    private:    

        std::function<void(IPhysicsCollider* other)> m_onContactBegin;
        std::function<void(IPhysicsCollider* other)> m_onContactEnd;
        friend class PhysicsBody;
    };

    class PolygonCollider : public IPhysicsCollider
    {
    public:
    private:
        PolygonCollider(bool isTrigger) : IPhysicsCollider(isTrigger) {}
        friend class PhysicsBody;
    };

    class BoxCollider : public IPhysicsCollider
    {
    public:
        void SetSize(Vector2 size);
        Vector2 GetSize() const { return m_size; }

    private:
        BoxCollider (bool isTrigger) : IPhysicsCollider(isTrigger){}
        Vector2 m_size = { 1.0f, 1.0f };
        friend class PhysicsBody;
    };

    class CircleCollider : public IPhysicsCollider
    {
    public:
        void SetRadius(float radius);
        float GetRadius() const { return m_radius; }

    private:
        CircleCollider (bool isTrigger) : IPhysicsCollider(isTrigger){}
        float m_radius = 1.0f;
        friend class PhysicsBody;
    };
} // namespace pgn