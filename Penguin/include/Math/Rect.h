#pragma once
#include "Vector.h"
#include "MathUtils.h"

namespace pgn {

    template <typename T>
    class Rect 
    {
    public:
        T x, y, w, h;

        Rect() : x(0), y(0), w(0), h(0) {}
        Rect(T x, T y, T w, T h) : x(x), y(y), w(w), h(h) {}
        Rect(Vector2 pos, Vector2 size) : x(pos.x), y(pos.y), w(size.x), h(size.y) {}

        // Convert from other types (e.g., Rect<int> to Rect<float>)
        template <typename U>
        explicit Rect(const Rect<U>& other) 
            : x(static_cast<T>(other.x))
            , y(static_cast<T>(other.y))
            , w(static_cast<T>(other.w))
            , h(static_cast<T>(other.h)) {}

        // Check if a point is inside
        bool contains(T px, T py) const { return (px >= x && px < x + w && py >= y && py < y + h); }
        bool contains(Vector2 point) const { return contains(point.x, point.y); }

        // Check for intersection with another rect
        bool intersects(const Rect<T>& other) const { return (x < other.x + other.w && x + w > other.x && y < other.y + other.h && y + h > other.y); }

        //get intersection area with another rect
        Rect<T> intersection(const Rect<T>& other) const 
        {
            float x1 = Math::Max(x, other.x);
            float y1 = Math::Max(y, other.y);
            float x2 = Math::Min(x + w, other.x + other.w);
            float y2 = Math::Min(y + h, other.y + other.h);

            if (x2 >= x1 && y2 >= y1)
                return { x1, y1, x2 - x1, y2 - y1 };
            
            return { 0, 0, 0, 0 }; // No intersection
        }

        Vector2 getSize() const { return {(float)w, (float)h}; }
        Vector2 getPosition() const { return {(float)x, (float)y}; }
    };

    using IntRect   = Rect<int>;
    using FloatRect = Rect<float>;

} // namespace pgn