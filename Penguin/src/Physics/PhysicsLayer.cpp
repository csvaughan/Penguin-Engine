#include "Physics/PhysicsLayer.h"
#include "Log/Log.h"

namespace pgn
{
    void PhysicsLayer::RegisterLayer(const std::string& name)
    {
        auto& instance = Get(); 

        if (instance.m_layers.find(name) != instance.m_layers.end()) 
        {
            PGN_CORE_WARN("Physics layer '{0}' is already registered.", name); 
            return; 
        }

        if (instance.m_currentBitIndex >= 64) 
        {
            PGN_CORE_WARN("Maximum number of physics layers (64) reached. Cannot register '{0}'.", name); 
            return; 
        } 
        
        uint64_t layerBit = 1ULL << instance.m_currentBitIndex; 
        instance.m_layers[name] = layerBit; 
        instance.m_currentBitIndex++; 

        // Default: New layers collide with EVERYTHING (~0ULL is all bits set to 1)
        instance.m_collisionMasks[name] = ~0ULL;
    }

    // Sets whether two layers should collide with each other (Symmetric)
    void PhysicsLayer::SetLayerCollision(const std::string& layerA, const std::string& layerB, bool shouldCollide)
    {
        auto& instance = Get();
        
        auto itA = instance.m_layers.find(layerA);
        auto itB = instance.m_layers.find(layerB);

        
        if (itA == instance.m_layers.end() || itB == instance.m_layers.end())
        {
            if (itA == instance.m_layers.end()) PGN_CORE_WARN("Cannot set collision: '{0}' is not registered.", layerA);
            if (itB == instance.m_layers.end()) PGN_CORE_WARN("Cannot set collision: '{0}' is not registered.", layerB);   
            return;
        }

        uint64_t bitA = itA->second;
        uint64_t bitB = itB->second;

        if (shouldCollide)
        {
            instance.m_collisionMasks[layerA] |= bitB;  // Enable B in A's mask
            instance.m_collisionMasks[layerB] |= bitA;  // Enable A in B's mask
        }
        else
        {
            instance.m_collisionMasks[layerA] &= ~bitB; // Disable B in A's mask
            instance.m_collisionMasks[layerB] &= ~bitA; // Disable A in B's mask
        }
    }

    uint64_t PhysicsLayer::GetLayerMask(const std::string& name) 
    {
        auto& layers = Get().m_layers; 
        auto it = layers.find(name); 
        if (it != layers.end()) return it->second; 
        
        PGN_CORE_WARN("Physics layer '{0}' does not exist!", name);
        return 1ULL << 0; 
    }

    // Returns the 64-bit combination of what this layer can hit
    uint64_t PhysicsLayer::GetCollisionMask(const std::string& name)
    {
        auto& instance = Get();
        auto it = instance.m_collisionMasks.find(name);
        if (it != instance.m_collisionMasks.end()) return it->second;

        return ~0ULL; // Default to colliding with everything if not found
    }

    void PhysicsLayer::Clear() 
    {
        auto& instance = Get(); 
        instance.m_layers.clear(); 
        instance.m_collisionMasks.clear();
        instance.m_currentBitIndex = 0; 
    }
} // namespace pgn
