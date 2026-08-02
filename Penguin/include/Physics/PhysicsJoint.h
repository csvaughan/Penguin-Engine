#pragma once

namespace pgn {

    enum class PhysicsJointType
    {
        Distance,
        Revolute,
        Prismatic
    };

    class PhysicsJoint
    {
    public:
        virtual ~PhysicsJoint() = default;

        PhysicsJointType GetType() const { return m_type; }

        void SetCollideConnected(bool shouldCollide);
        bool GetCollideConnected() const;

    protected:

        PhysicsJoint(uint64_t jointId, PhysicsJointType type) : m_jointId(jointId), m_type(type) {}
        
        uint64_t m_jointId;
        PhysicsJointType m_type;
        friend class PhysicsWorld;
    };

    // --- Distance Joint ---
    class DistanceJoint : public PhysicsJoint
    {
    public:
       
        void SetLength(float length);
        float GetLength() const;
        void EnableLimit(bool enable);
        void SetMinMaxLength(float minLen, float maxLen);
    
    private:
        DistanceJoint(uint64_t jointId) : PhysicsJoint(jointId, PhysicsJointType::Distance) {}
        friend class PhysicsWorld;
    };

    // --- Revolute Joint (Hinge) ---
    class RevoluteJoint : public PhysicsJoint
    {
    public:

        void EnableMotor(bool enable);
        bool IsMotorEnabled() const;
        void SetMotorSpeed(float speedInRadians);
        float GetMotorSpeed() const;
        void SetMaxMotorTorque(float torque);

        void EnableLimit(bool enable);
        bool IsLimitEnabled() const;
        void SetLimits(float lowerAngleRad, float upperAngleRad);
    
    private:
        RevoluteJoint(uint64_t jointId) : PhysicsJoint(jointId, PhysicsJointType::Revolute) {}
        friend class PhysicsWorld;
    };

    // --- Prismatic Joint (Slider) ---
    class PrismaticJoint : public PhysicsJoint
    {
    public:
        void EnableMotor(bool enable);
        bool IsMotorEnabled() const;
        void SetMotorSpeed(float speedMetersPerSec);
        float GetMotorSpeed() const;
        void SetMaxMotorForce(float force);

        void EnableLimit(bool enable);
        bool IsLimitEnabled() const;
        void SetLimits(float lowerTranslation, float upperTranslation);
    
    private:
        PrismaticJoint(uint64_t jointId) : PhysicsJoint(jointId, PhysicsJointType::Prismatic) {}
        friend class PhysicsWorld;
    };
}