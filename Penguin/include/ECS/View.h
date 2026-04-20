#pragma once
#include "Defs.h"

namespace pgn::ECS
{

    class Registry;

    template<typename... Components>
    class View 
    {
    public:
        View(Registry* registry) : m_registry(registry) {
            // Build the required bitmask using our Static IDs
            (m_requiredSignature.set(ComponentIdGenerator::GetId<Components>()), ...);
        }

        struct Iterator {
            Registry* registry;
            uint32_t index;
            Signature required;

            // Move to the next valid entity matching the signature
            Iterator& operator++() {
                index++;
                while (index < MAX_ENTITIES && !IsMatch()) {
                    index++;
                }
                return *this;
            }

            bool IsMatch() const;
            bool operator!=(const Iterator& other) const { return index != other.index; }
            auto operator*();
        };

        Iterator begin() {
            Iterator it{ m_registry, 0, m_requiredSignature };
            // If the first entity doesn't match, find the first one that does
            if (it.index < MAX_ENTITIES && !it.IsMatch()) ++it;
            return it;
        }

        Iterator end() {
            return Iterator{ m_registry, MAX_ENTITIES, m_requiredSignature };
        }

    private:
        Registry* m_registry;
        Signature m_requiredSignature;
    };
} // namespace pgn
