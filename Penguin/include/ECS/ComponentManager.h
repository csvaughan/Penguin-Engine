#pragma once
#include "Defs.h"
#include "ComponentArray.h"
#include "Log/Log.h"
#include <array>
#include <memory>

namespace pgn::ECS
{
    class ComponentManager
    {
    public:
        template<typename T>
        void RegisterComponent()
        {
            ComponentType typeId = ComponentIdGenerator::GetId<T>();

            PGN_ASSERT(typeId < MAX_COMPONENTS, "Exceeded maximum component types.");
            PGN_ASSERT(m_ComponentArrays[typeId] == nullptr, "Registering component type more than once.");

            // Create the specialized ComponentArray (Sparse Set) for this type
            m_ComponentArrays[typeId] = std::make_shared<ComponentArray<T>>();
        }

        template<typename T>
        ComponentType GetComponentType()
        {
            return ComponentIdGenerator::GetId<T>();
        }

        template<typename T, typename... Args>
        void AddComponent(EntityID entity, Args&&... args)
        {
            GetComponentArray<T>()->InsertData(entity, T(std::forward<Args>(args)...));
        }

        template<typename T>
        void RemoveComponent(EntityID entity)
        {
            GetComponentArray<T>()->RemoveData(entity);
        }

        template<typename T>
        T& GetComponent(EntityID entity)
        {
            return GetComponentArray<T>()->GetData(entity);
        }

        // Called when an entity is destroyed to wipe all its component data
        void EntityDestroyed(EntityID entity)
        {
            for (auto const& componentArray : m_ComponentArrays)
            {
                if (componentArray)
                {
                    componentArray->EntityDestroyed(entity);
                }
            }
        }

    private:
        // Internal helper to get the typed pointer from the base interface
        template<typename T>
        std::shared_ptr<ComponentArray<T>> GetComponentArray()
        {
            ComponentType typeId = ComponentIdGenerator::GetId<T>();
            PGN_ASSERT(m_ComponentArrays[typeId] != nullptr, "Component not registered before use.");
            
            return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeId]);
        }

    private:
        std::array<std::shared_ptr<IComponentArray>, MAX_COMPONENTS> m_ComponentArrays{};
    };
}