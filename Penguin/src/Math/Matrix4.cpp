#include "Math/Matrix4.h"
#include "Math/MathUtils.h"
#include <cmath>

namespace pgn {

    // Default constructor: Initializes to an Identity Matrix
    Matrix4::Matrix4() 
    {
        for (int i = 0; i < 16; ++i) m[i] = 0.0f;
        m[0] = 1.0f; m[5] = 1.0f; m[10] = 1.0f; m[15] = 1.0f;
    }

    // --- Static Factories ---

    Matrix4 Matrix4::Translate(const Vector3& translation) 
    {
        Matrix4 res; // Identity
        res.m[12] = translation.x;
        res.m[13] = translation.y;
        res.m[14] = translation.z;
        return res;
    }

    Matrix4 Matrix4::Rotate(float degrees, const Vector3& axis) 
    {
        float radians = Math::ToRadians(degrees);
        float c = Math::Cos(radians);
        float s = Math::Sin(radians);
        float t = 1.0f - c;

        // Normalize the axis vector
        float len = std::sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
        if (Math::Approximately(len, 0.0f)) return Matrix4();
        
        float x = axis.x / len;
        float y = axis.y / len;
        float z = axis.z / len;

        Matrix4 res;
        // Column 0
        res.m[0] = t * x * x + c;
        res.m[1] = t * x * y + s * z;
        res.m[2] = t * x * z - s * y;
        res.m[3] = 0.0f;

        // Column 1
        res.m[4] = t * x * y - s * z;
        res.m[5] = t * y * y + c;
        res.m[6] = t * y * z + s * x;
        res.m[7] = 0.0f;

        // Column 2
        res.m[8] = t * x * z + s * y;
        res.m[9] = t * y * z - s * x;
        res.m[10] = t * z * z + c;
        res.m[11] = 0.0f;

        // Column 3
        res.m[12] = 0.0f;
        res.m[13] = 0.0f;
        res.m[14] = 0.0f;
        res.m[15] = 1.0f;

        return res;
    }

    Matrix4 Matrix4::Scale(const Vector3& scale) 
    {
        Matrix4 res; // Identity
        res.m[0] = scale.x;
        res.m[5] = scale.y;
        res.m[10] = scale.z;
        return res;
    }

    Matrix4 Matrix4::Orthographic(float left, float right, float bottom, float top, float zNear, float zFar) 
    {
        Matrix4 res;
        res.m[0] = 2.0f / (right - left);
        res.m[5] = 2.0f / (top - bottom);
        res.m[10] = -2.0f / (zFar - zNear);
        
        res.m[12] = -(right + left) / (right - left);
        res.m[13] = -(top + bottom) / (top - bottom);
        res.m[14] = -(zFar + zNear) / (zFar - zNear);
        res.m[15] = 1.0f;
        return res;
    }

    // --- Instance Methods ---

    Matrix4 Matrix4::Invert() const 
    {
        float inv[16];

        inv[0] = m[5]  * m[10] * m[15] - m[5]  * m[11] * m[14] - m[9]  * m[6]  * m[15] + 
                 m[9]  * m[7]  * m[14] + m[13] * m[6]  * m[11] - m[13] * m[7]  * m[10];

        inv[4] = -m[4]  * m[10] * m[15] + m[4]  * m[11] * m[14] + m[8]  * m[6]  * m[15] - 
                  m[8]  * m[7]  * m[14] - m[12] * m[6]  * m[11] + m[12] * m[7]  * m[10];

        inv[8] = m[4]  * m[9]  * m[15] - m[4]  * m[11] * m[13] - m[8]  * m[5]  * m[15] + 
                 m[8]  * m[7]  * m[13] + m[12] * m[5]  * m[11] - m[12] * m[7]  * m[9];

        inv[12] = -m[4]  * m[9]  * m[14] + m[4]  * m[10] * m[13] + m[8]  * m[5]  * m[14] - 
                   m[8]  * m[6]  * m[13] - m[12] * m[5]  * m[10] + m[12] * m[6]  * m[9];

        inv[1] = -m[1]  * m[10] * m[15] + m[1]  * m[11] * m[14] + m[9]  * m[2]  * m[15] - 
                  m[9]  * m[3]  * m[14] - m[13] * m[2]  * m[11] + m[13] * m[3]  * m[10];

        inv[5] = m[0]  * m[10] * m[15] - m[0]  * m[11] * m[14] - m[8]  * m[2]  * m[15] + 
                 m[8]  * m[3]  * m[14] + m[12] * m[2]  * m[11] - m[12] * m[3]  * m[10];

        inv[9] = -m[0]  * m[9]  * m[15] + m[0]  * m[11] * m[13] + m[8]  * m[1]  * m[15] - 
                  m[8]  * m[3]  * m[13] - m[12] * m[1]  * m[11] + m[12] * m[3]  * m[9];

        inv[13] = m[0]  * m[9]  * m[14] - m[0]  * m[10] * m[13] - m[8]  * m[1]  * m[14] + 
                  m[8]  * m[2]  * m[13] + m[12] * m[1]  * m[10] - m[12] * m[2]  * m[9];

        inv[2] = m[1]  * m[6] * m[15] - m[1]  * m[7] * m[14] - m[5]  * m[2] * m[15] + 
                 m[5]  * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];

        inv[6] = -m[0]  * m[6] * m[15] + m[0]  * m[7] * m[14] + m[4]  * m[2] * m[15] - 
                  m[4]  * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];

        inv[10] = m[0]  * m[5] * m[15] - m[0]  * m[7] * m[13] - m[4]  * m[1] * m[15] + 
                  m[4]  * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

        inv[14] = -m[0]  * m[5] * m[14] + m[0]  * m[6] * m[13] + m[4]  * m[1] * m[14] - 
                   m[4]  * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

        inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - 
                  m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];

        inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + 
                 m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];

        inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - 
                   m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

        inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + 
                  m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

        float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

        if (Math::Approximately(det, 0.0f)) return Matrix4(); // Singular matrix

        float invDet = 1.0f / det;
        Matrix4 result;
        for (int i = 0; i < 16; i++) result.m[i] = inv[i] * invDet;

        return result;
    }

    // --- Operators ---

    Matrix4& Matrix4::operator*=(const Matrix4& other) 
    {
        Matrix4 result;
        for (int col = 0; col < 4; ++col) 
        {
            for (int row = 0; row < 4; ++row) 
            {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) 
                {
                    sum += this->m[k * 4 + row] * other.m[col * 4 + k];
                }
                result.m[col * 4 + row] = sum;
            }
        }
        *this = result;
        return *this;
    }

    Matrix4& Matrix4::operator*=(float scalar) 
    {
        for (int i = 0; i < 16; ++i) m[i] *= scalar;
        return *this;
    }

    Vector4 Matrix4::operator*(const Vector4& v) const 
    {
        return {
            m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12] * v.w,
            m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13] * v.w,
            m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w,
            m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w
        };
    }

    bool Matrix4::operator==(const Matrix4& other) const 
    {
        for (int i = 0; i < 16; ++i) 
        { 
            if (!Math::Approximately(m[i], other.m[i])) return false; 
        }
        return true;
    }

} // namespace pgn