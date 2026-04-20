#pragma once
#include "Event.h"
#include "EventBus.h"

namespace pgn
{
    struct EventToken 
    {
        EventType Type;
        size_t ID;

        EventToken(EventType type, size_t id) : Type(type), ID(id) {}
        ~EventToken() { EventBus::Unsubscribe(Type, ID); }
        
        // Disable copying to prevent accidental double-unsubscribing
        EventToken(const EventToken&) = delete;
        EventToken& operator=(const EventToken&) = delete;
    };
} // namespace pgn