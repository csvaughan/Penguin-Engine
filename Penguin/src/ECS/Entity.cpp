#include "ECS/Entity.h"
#include "ECS/Registry.h"

namespace pgn::ECS
{
    void Entity::Destroy()
    {
        if (IsValid()) 
        {
            m_registry->DestroyEntity(m_id);
            m_registry = nullptr; 
        }
    }

    bool Entity::IsValid() const { return m_registry && m_registry->IsEntityAlive(m_id); }

    TransformComponent& Entity::GetTransform() { return GetComponent<TransformComponent>(); }
}