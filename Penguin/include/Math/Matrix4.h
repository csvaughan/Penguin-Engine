#pragma once
#include "Vector.h"

namespace pgn {

    // Mixin for multiplication (Matrix * Matrix and Matrix * Scalar)
    template <typename T>
    struct MatrixMultiplicative {
        friend T operator*(T a, const T& b) { return a *= b; }
        friend T operator*(T a, float s)    { return a *= s; }
        friend T operator*(float s, T a)    { return a *= s; }
    };

    struct Matrix4 : MatrixMultiplicative<Matrix4>
    {
        float m[16]; 

        Matrix4(); // Defaults to Identity

        // --- Transformation Helpers ---
        static Matrix4 Translate(const Vector3& translation);
        static Matrix4 Rotate(float degrees, const Vector3& axis);
        static Matrix4 Scale(const Vector3& scale);
        static Matrix4 Orthographic(float left, float right, float bottom, float top, float zNear = -1.0f, float zFar = 1.0f);

        Matrix4 Invert() const;

        // --- Core Operators ---
        Matrix4& operator*=(const Matrix4& other);
        Matrix4& operator*=(float scalar);
        Vector4  operator*(const Vector4& v) const; 

        bool operator==(const Matrix4& other) const;
    };

} // namespace pgn