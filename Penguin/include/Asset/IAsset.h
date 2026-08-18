#pragma once
#include "Memory/Ref.h"

namespace pgn
{
    class IAsset : public RefCounted {
    public:
        virtual ~IAsset() = default;
    };
} // namespace pgn
