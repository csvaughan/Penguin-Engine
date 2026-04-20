#pragma once
#include "pgpch.h"

namespace pgn::ECS {

    struct EntityID 
    {
        uint32_t index : 20;
        uint32_t generation : 12;

        bool operator==(const EntityID& other) const { return index == other.index && generation == other.generation;}
    };

    using ComponentType = uint8_t;
    const uint32_t MAX_ENTITIES = 10000;
    const uint8_t MAX_COMPONENTS = 32;

    using Signature = std::bitset<MAX_COMPONENTS>;

    class ComponentIdGenerator 
    {
    private:
        static inline ComponentType s_nextId = 0;
    public:
        template<typename T>
        static ComponentType GetId() 
        {
            static const ComponentType id = s_nextId++;
            return id;
        }
    };
}