#pragma once
#include "Defs.h"

namespace pgn::ECS {
    class IComponentArray {
    public:
        virtual ~IComponentArray() = default;
        virtual void EntityDestroyed(EntityID entity) = 0;
    };

    template<typename T>
    class ComponentArray : public IComponentArray {
    public:
        ComponentArray() { m_sparseTable.fill(std::numeric_limits<size_t>::max()); }

        void InsertData(EntityID entity, T component) 
        {
            size_t newIndex = m_denseArray.size();
            m_sparseTable[entity.index] = newIndex;
            m_indexToEntityMap[newIndex] = entity;
            m_denseArray.push_back(std::move(component));
        }

        void RemoveData(EntityID entity) 
        {
            size_t indexOfRemovedEntity = m_sparseTable[entity.index];
            size_t indexOfLastElement = m_denseArray.size() - 1;

            // Move last element to the deleted spot to keep dense array packed
            m_denseArray[indexOfRemovedEntity] = std::move(m_denseArray[indexOfLastElement]);

            // Update maps
            EntityID entityOfLastElement = m_indexToEntityMap[indexOfLastElement];
            m_sparseTable[entityOfLastElement.index] = indexOfRemovedEntity;
            m_indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

            m_sparseTable[entity.index] = std::numeric_limits<size_t>::max();
            m_denseArray.pop_back();
        }

        T& GetData(EntityID entity) { return m_denseArray[m_sparseTable[entity.index]]; }

        void EntityDestroyed(EntityID entity) override 
        {
            if (m_sparseTable[entity.index] != std::numeric_limits<size_t>::max()) 
                RemoveData(entity);
        }

        // For high-speed iteration
        const std::vector<T>& GetRawData() const { return m_denseArray; }

    private:
        std::vector<T> m_denseArray; 
        std::array<size_t, MAX_ENTITIES> m_sparseTable;
        std::array<EntityID, MAX_ENTITIES> m_indexToEntityMap;
    };
}