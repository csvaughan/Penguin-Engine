#pragma once
#include "Matrix4.h"

namespace pgn {

    struct Transform2D 
    {
        Vector2 position = {0,0};
        float rotation = 0;
        Vector2 scale = {1,1};
        Vector2 origin = {0,0};

        [[nodiscard]] Matrix4 GetModelMatrix() const 
        {
            return Matrix4::Translate({position.x, position.y, 0.0f}) *
                Matrix4::Rotate(rotation, {0.0f, 0.0f, 1.0f}) *
                Matrix4::Scale({scale.x, scale.y, 1.0f});
        }
    };
}