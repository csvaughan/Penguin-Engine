#include "ECS/View.h"
#include "ECS/Registry.h"

namespace pgn::ECS
{
    template <typename... Components>
    inline bool pgn::ECS::View<Components...>::Iterator::IsMatch() const
    {
        // We use the raw index to check the signature
            // because signatures are indexed by the entity's "slot"
            auto entitySig = registry->m_entityManager.GetSignature({index, 0});
            return (entitySig & required) == required;
    }

    template <typename... Components>
    auto pgn::ECS::View<Components...>::Iterator::operator*()
    {
        // We fetch the current generation to create a valid ID handle
        uint32_t gen = registry->m_entityManager.GetGeneration(index);
        EntityID id = { .index = index, .generation = gen };
        
        // Returns a tuple of references (O(1) lookups via Sparse Sets)
        return std::forward_as_tuple(registry->GetComponent<Components>(id)...);
    }
} // namespace pgn::ECS