#include "Physics/PhysicsJoint.h"
#include "Physics/PhysicsInternal.h"

namespace pgn
{
    // --- Base Joint Properties ---
    void PhysicsJoint::SetCollideConnected(bool shouldCollide)
    {
        b2Joint_SetCollideConnected(ToB2Joint(m_jointId), shouldCollide); 
    }

    bool PhysicsJoint::GetCollideConnected() const
    {
        return b2Joint_GetCollideConnected(ToB2Joint(m_jointId)); 
    }

    // --- Distance Joint Implementation ---
    void DistanceJoint::SetLength(float length) { b2DistanceJoint_SetLength(ToB2Joint(m_jointId), length); }
    float DistanceJoint::GetLength() const { return b2DistanceJoint_GetLength(ToB2Joint(m_jointId)); }
    void DistanceJoint::EnableLimit(bool enable) { b2DistanceJoint_EnableLimit(ToB2Joint(m_jointId), enable); }
    void DistanceJoint::SetMinMaxLength(float minLen, float maxLen) { b2DistanceJoint_SetLengthRange(ToB2Joint(m_jointId), minLen, maxLen); }

    // --- Revolute Joint Implementation ---
    void RevoluteJoint::EnableMotor(bool enable) { b2RevoluteJoint_EnableMotor(ToB2Joint(m_jointId), enable); } 
    bool RevoluteJoint::IsMotorEnabled() const { return b2RevoluteJoint_IsMotorEnabled(ToB2Joint(m_jointId)); } 
    void RevoluteJoint::SetMotorSpeed(float speed) { b2RevoluteJoint_SetMotorSpeed(ToB2Joint(m_jointId), speed); } 
    float RevoluteJoint::GetMotorSpeed() const { return b2RevoluteJoint_GetMotorSpeed(ToB2Joint(m_jointId)); } 
    void RevoluteJoint::SetMaxMotorTorque(float torque) { b2RevoluteJoint_SetMaxMotorTorque(ToB2Joint(m_jointId), torque); } 

    void RevoluteJoint::EnableLimit(bool enable) { b2RevoluteJoint_EnableLimit(ToB2Joint(m_jointId), enable); } 
    bool RevoluteJoint::IsLimitEnabled() const { return b2RevoluteJoint_IsLimitEnabled(ToB2Joint(m_jointId)); } 
    void RevoluteJoint::SetLimits(float lower, float upper) { b2RevoluteJoint_SetLimits(ToB2Joint(m_jointId), lower, upper); } 

    // --- Prismatic Joint Implementation ---
    void PrismaticJoint::EnableMotor(bool enable) { b2PrismaticJoint_EnableMotor(ToB2Joint(m_jointId), enable); } 
    bool PrismaticJoint::IsMotorEnabled() const { return b2PrismaticJoint_IsMotorEnabled(ToB2Joint(m_jointId)); } 
    void PrismaticJoint::SetMotorSpeed(float speed) { b2PrismaticJoint_SetMotorSpeed(ToB2Joint(m_jointId), speed); } 
    float PrismaticJoint::GetMotorSpeed() const { return b2PrismaticJoint_GetMotorSpeed(ToB2Joint(m_jointId)); } 
    void PrismaticJoint::SetMaxMotorForce(float force) { b2PrismaticJoint_SetMaxMotorForce(ToB2Joint(m_jointId), force); } 

    void PrismaticJoint::EnableLimit(bool enable) { b2PrismaticJoint_EnableLimit(ToB2Joint(m_jointId), enable); } 
    bool PrismaticJoint::IsLimitEnabled() const { return b2PrismaticJoint_IsLimitEnabled(ToB2Joint(m_jointId)); } 
    void PrismaticJoint::SetLimits(float lower, float upper) { b2PrismaticJoint_SetLimits(ToB2Joint(m_jointId), lower, upper); } 
} 