#pragma once
#include "Defs.h"

namespace pgn::ECS {

    class EntityManager 
    {
    public:
        EntityManager() 
        {
            for (uint32_t i = 0; i < MAX_ENTITIES; ++i) m_freeIDs.push(i);
            m_generations.fill(0);
        }

        EntityID CreateEntity() 
        {
            uint32_t index = m_freeIDs.front();
            m_freeIDs.pop();
            return { .index = index, .generation = m_generations[index] };
        }

        void DestroyEntity(EntityID id) 
        {
            m_generations[id.index]++; // Invalidate all existing handles
            m_freeIDs.push(id.index);
            m_signatures[id.index].reset();
        }

        bool IsAlive(EntityID id) const { return m_generations[id.index] == id.generation; }
        
        void SetSignature(EntityID id, Signature sig) { m_signatures[id.index] = sig; }
        Signature GetSignature(EntityID id) const { return m_signatures[id.index]; }
        uint32_t GetGeneration(uint32_t index) const { return m_generations[index]; }

    private:
        std::queue<uint32_t> m_freeIDs;
        std::array<uint32_t, MAX_ENTITIES> m_generations;
        std::array<Signature, MAX_ENTITIES> m_signatures;
    };
}