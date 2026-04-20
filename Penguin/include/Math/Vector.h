#pragma once
#include <iosfwd>

namespace pgn {

    // --- Arithmetic Mixins ---
    template <typename T>
    struct Additive {
        friend T operator+(T a, const T& b) { return a += b; }
        friend T operator-(T a, const T& b) { return a -= b; }
    };

    template <typename T>
    struct Multiplicative {
        friend T operator*(T v, float s) { return v *= s; }
        friend T operator*(float s, T v) { return v *= s; }
        friend T operator/(T v, float s) { return v /= s; }
    };

    // --- Vector 4 ---
    struct Vector4 : Additive<Vector4>, Multiplicative<Vector4> 
    {
        float x, y, z, w;

        Vector4(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f) 
            : x(_x), y(_y), z(_z), w(_w) {}

        // Constants
        static Vector4 Zero() { return { 0.0f, 0.0f, 0.0f, 0.0f }; }
        static Vector4 One()  { return { 1.0f, 1.0f, 1.0f, 1.0f }; }

        // Assignments
        Vector4& operator+=(const Vector4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
        Vector4& operator-=(const Vector4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
        Vector4& operator*=(float s)          { x *= s; y *= s; z *= s; w *= s; return *this; }
        Vector4& operator/=(float s)          { x /= s; y /= s; z /= s; w /= s; return *this; }

        bool operator==(const Vector4& v) const;

        friend std::ostream& operator<<(std::ostream& os, const Vector4& v);
    };

    // --- Vector 3 ---
    struct Vector3 : Additive<Vector3>, Multiplicative<Vector3> 
    {
        float x, y, z;

        Vector3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}
        Vector3(float scalar) : x(scalar), y(scalar), z(scalar) {}

        // Static Constants
        static Vector3 Zero()    { return { 0.0f,  0.0f,  0.0f }; }
        static Vector3 One()     { return { 1.0f,  1.0f,  1.0f }; }
        static Vector3 Up()      { return { 0.0f,  1.0f,  0.0f }; }
        static Vector3 Down()    { return { 0.0f, -1.0f,  0.0f }; }
        static Vector3 Left()    { return {-1.0f,  0.0f,  0.0f }; }
        static Vector3 Right()   { return { 1.0f,  0.0f,  0.0f }; }
        static Vector3 Forward() { return { 0.0f,  0.0f,  1.0f }; }
        static Vector3 Back()    { return { 0.0f,  0.0f, -1.0f }; }

        // Logic
        float LengthSquared() const { return x * x + y * y + z * z; }
        float Length() const;
        Vector3 Normalize() const   { float len = Length(); return (len > 0) ? *this / len : Zero(); }
        static float Dot(const Vector3& a, const Vector3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
        static Vector3 Cross(const Vector3& a, const Vector3& b) { return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; }

        // Assignments
        Vector3& operator+=(const Vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
        Vector3& operator-=(const Vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
        Vector3& operator*=(float s)          { x *= s; y *= s; z *= s; return *this; }
        Vector3& operator/=(float s)          { x /= s; y /= s; z /= s; return *this; }

        bool operator==(const Vector3& v) const;

        friend std::ostream& operator<<(std::ostream& os, const Vector3& v);
    };

    // --- Vector 2 ---
    struct Vector2 : Additive<Vector2>, Multiplicative<Vector2> 
    {
        float x, y;

        Vector2(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
        Vector2(float scalar) : x(scalar), y(scalar) {}
        Vector2(const Vector4& v) : x(v.x), y(v.y) {}

        // Static Constants
        static Vector2 Zero()  { return { 0.0f,  0.0f }; }
        static Vector2 One()   { return { 1.0f,  1.0f }; }
        static Vector2 Up()    { return { 0.0f, -1.0f }; } // Screen space convention
        static Vector2 Down()  { return { 0.0f,  1.0f }; }
        static Vector2 Left()  { return {-1.0f,  0.0f }; }
        static Vector2 Right() { return { 1.0f,  0.0f }; }
        static Vector2 UnitX() { return { 1.0f,  0.0f }; }
        static Vector2 UnitY() { return { 0.0f,  1.0f }; }

        // Logic
        float LengthSquared() const { return x * x + y * y; }
        float Length() const;
        Vector2 Normalize() const   { float len = Length(); return (len > 0) ? *this / len : Zero(); }
        static float Dot(const Vector2& a, const Vector2& b) { return a.x * b.x + a.y * b.y; }
        static float Cross(const Vector2& a, const Vector2& b) { return a.x * b.y - a.y * b.x; }

        // Assignments
        Vector2& operator+=(const Vector2& v) { x += v.x; y += v.y; return *this; }
        Vector2& operator-=(const Vector2& v) { x -= v.x; y -= v.y; return *this; }
        Vector2& operator*=(float s)          { x *= s; y *= s; return *this; }
        Vector2& operator/=(float s)          { x /= s; y /= s; return *this; }

        bool operator==(const Vector2& v) const;

        friend std::ostream& operator<<(std::ostream& os, const Vector2& v);
    };

} // namespace pgn