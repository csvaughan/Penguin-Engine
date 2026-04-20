#pragma once
#include "Components.h"
#include "Defs.h"

namespace pgn::ECS 
{
    class Registry; 

    class Entity 
    {
    public:
        Entity() : m_id({0, 0}), m_registry(nullptr) {}
        
        bool IsValid() const;
        void Destroy();
       
        template<typename T, typename... Args>
        void AddComponent(Args&&... args);

        template<typename T>
        void RemoveComponent();

        template<typename T>
        T& GetComponent();

        TransformComponent& GetTransform();

    private:
        friend class Registry;
        Entity(EntityID id, Registry* registry) : m_id(id), m_registry(registry) {}

        EntityID m_id;
        Registry* m_registry;
    };
}