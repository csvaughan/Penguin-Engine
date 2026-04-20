#pragma once
#include "EntityManager.h"
#include "ComponentManager.h" 
#include "Entity.h"
#include "View.h"

namespace pgn::ECS {

    class Registry
    {
    public:
        Entity CreateEntity() 
        {
            Entity entity(m_entityManager.CreateEntity(), this);

            // Always ensure a Transform exists
            entity.AddComponent<TransformComponent>();
            
            return entity;
        }
        
        void DestroyEntity(EntityID entity) 
        {
            m_entityManager.DestroyEntity(entity);
            m_componentManager.EntityDestroyed(entity);
        }

        template<typename T, typename... Args>
        void AddComponent(EntityID entity, Args&&... args) 
        {
            m_componentManager.AddComponent<T>(entity, T(std::forward<Args>(args)...));
            auto sig = m_entityManager.GetSignature(entity);
            sig.set(ComponentIdGenerator::GetId<T>());
            m_entityManager.SetSignature(entity, sig);
        }

        template<typename T>
        T& GetComponent(EntityID entity) { return m_componentManager.GetComponent<T>(entity); }

        template<typename T>
        void RemoveComponent(EntityID entity)
        {
            // 1. Get the unique static ID for this component type
            ComponentType typeID = ComponentIdGenerator::GetId<T>();

            // 2. Remove the actual data from the Sparse Set
            m_componentManager.RemoveComponent<T>(entity);

            // 3. Update the entity's bitset signature
            Signature sig = m_entityManager.GetSignature(entity);
            sig.set(typeID, false); // Set bit to 0
            m_entityManager.SetSignature(entity, sig);
        }

        bool IsEntityAlive(EntityID id) {return m_entityManager.IsAlive(id); }

        template<typename... Components>
        auto GetView() { return View<Components...>(this); }

    private:
        EntityManager m_entityManager;
        ComponentManager m_componentManager;
        friend class Iterator;
    };


    //Entity Methods
    template<typename T, typename... Args>
    void Entity::AddComponent(Args&&... args) { m_registry->AddComponent<T>(m_id, std::forward<Args>(args)...); }

    template<typename T>
    void Entity::RemoveComponent() { m_registry->RemoveComponent<T>(m_id); }

    template<typename T>
    T& Entity::GetComponent() { return m_registry->GetComponent<T>(m_id); }
}