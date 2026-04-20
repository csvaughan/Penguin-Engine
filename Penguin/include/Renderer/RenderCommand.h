#pragma once
#include "VertexArray.h"

struct SDL_Texture;

namespace pgn {

    struct SpriteCommand   { SDL_Texture* texture; std::array<Vertex, 4> vertices; std::array<int, 6> indices; };
    struct GeometryCommand { SDL_Texture* texture; VertexArray vertexArray; };
    struct TextCommand     { SDL_Texture* fontAtlas; VertexArray vertexArray; };
    struct ScissorCommand  { FloatRect rect; bool enabled; };

    using RenderCommand = std::variant<SpriteCommand, GeometryCommand, TextCommand, ScissorCommand>;

    // --- The Core Render Item ---
    struct RenderItem 
    {
        int zIndex;            // Primary sort: Layer depth
        uint32_t stateGroupID; // Secondary sort: Keeps state changes in sequence
        uintptr_t sortKey;     // Tertiary sort: Texture pointer (for batching)
        RenderCommand command;

        bool operator<(const RenderItem& other) const {
            if (zIndex != other.zIndex) return zIndex < other.zIndex;
            if (stateGroupID != other.stateGroupID) return stateGroupID < other.stateGroupID;
            return sortKey < other.sortKey;
        }
    };
}