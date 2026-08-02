#pragma once

namespace pgn
{
    class PhysicsLayer
    {
    public:
        static void RegisterLayer(const std::string& name);

        // Sets whether two layers should collide with each other (Symmetric)
        static void SetLayerCollision(const std::string& layerA, const std::string& layerB, bool shouldCollide);
        static uint64_t GetLayerMask(const std::string& name);

        // Returns the 64-bit combination of what this layer can hit
        static uint64_t GetCollisionMask(const std::string& name);
        static void Clear();

    private:
        PhysicsLayer() = default; 

        std::unordered_map<std::string, uint64_t> m_layers; 
        std::unordered_map<std::string, uint64_t> m_collisionMasks; // Stores matrix rules
        uint8_t m_currentBitIndex = 0; 

        static PhysicsLayer& Get() { 
            static PhysicsLayer instance; 
            return instance; 
        }
    };
} // namespace pgn