#pragma once
#include "pgpch.h"
#include <string>
#include <iostream>

namespace pgn
{
    using EventType = uint32_t;

    enum EventCategory : uint32_t
    {
        None = 0,
        EventCategoryEngine         = 1 << 0,
        EventCategoryInput          = 1 << 1,
        EventCategoryKeyboard       = 1 << 2,
        EventCategoryMouse          = 1 << 3,
        EventCategoryMouseButton    = 1 << 4,
        EventCategoryWindow         = 1 << 5
    };

    namespace Internal {
        inline EventType GenerateNewEventType() {
            static EventType lastID = 0;
            return ++lastID;
        }
    }

    // --- Macros ---

    #define EVENT_CLASS_TYPE(type) \
        static pgn::EventType GetStaticType() { \
            static const pgn::EventType id = pgn::Internal::GenerateNewEventType(); \
            return id; \
        } \
        virtual pgn::EventType GetEventType() const override { return GetStaticType(); } \
        virtual const char* GetName() const override { return #type; }

    #define EVENT_CLASS_CATEGORY(category) \
        virtual int GetCategoryFlags() const override { return category; }


    class Event
    {
    public:
        virtual ~Event() = default;

        bool Handled = false;

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0; // New requirement for bitmask
        virtual std::string ToString() const { return GetName(); }

        // Helper to check if this event belongs to a specific category
        inline bool IsInCategory(EventCategory category) const {
            return GetCategoryFlags() & category;
        }
    };

    // --- Event Dispatcher ---

    class EventDispatcher
    {
    public:
        EventDispatcher(Event& event)
            : m_Event(event) {}

        /**
         * Tries to dispatch an event to a specific function.
         * If the types match, the function is executed and its return value
         * sets the 'Handled' state of the event.
         */
        template<typename T, typename F>
        bool Dispatch(const F& func)
        {
            if (m_Event.GetEventType() == T::GetStaticType() && !m_Event.Handled)
            {
                // We use |= so that once an event is Handled, it stays Handled
                m_Event.Handled |= func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };

    inline std::ostream& operator<<(std::ostream& os, const Event& e)
    {
        return os << e.ToString();
    }

} // namespace pgn