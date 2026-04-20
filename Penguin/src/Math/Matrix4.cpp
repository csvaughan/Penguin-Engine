#include "Math/Matrix4.h"
#include "Math/MathUtils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pgn {

    // Helper macros for GLM mapping
    #define AS_GLM(x) (*reinterpret_cast<glm::mat4*>(x))
    #define AS_CONST_GLM(x) (*reinterpret_cast<const glm::mat4*>(x))

    Matrix4::Matrix4() { AS_GLM(m) = glm::mat4(1.0f); }

    // --- Static Factories ---

    Matrix4 Matrix4::Translate(const Vector3& v) 
    {
        Matrix4 res;
        AS_GLM(res.m) = glm::translate(glm::mat4(1.0f), glm::vec3(v.x, v.y, v.z));
        return res;
    }

    Matrix4 Matrix4::Rotate(float degrees, const Vector3& axis) 
    {
        Matrix4 res;
        AS_GLM(res.m) = glm::rotate(glm::mat4(1.0f), Math::ToRadians(degrees), glm::vec3(axis.x, axis.y, axis.z));
        return res;
    }

    Matrix4 Matrix4::Scale(const Vector3& v) 
    {
        Matrix4 res;
        AS_GLM(res.m) = glm::scale(glm::mat4(1.0f), glm::vec3(v.x, v.y, v.z));
        return res;
    }

    Matrix4 Matrix4::Orthographic(float l, float r, float b, float t, float n, float f) 
    {
        Matrix4 res;
        AS_GLM(res.m) = glm::ortho(l, r, b, t, n, f);
        return res;
    }

    // --- Instance Methods ---

    Matrix4 Matrix4::Invert() const 
    {
        Matrix4 result;
        AS_GLM(result.m) = glm::inverse(AS_CONST_GLM(this->m));
        return result;
    }

    // --- Operators ---

    Matrix4& Matrix4::operator*=(const Matrix4& other) 
    {
        AS_GLM(m) = AS_CONST_GLM(m) * AS_CONST_GLM(other.m);
        return *this;
    }

    Matrix4& Matrix4::operator*=(float scalar) { AS_GLM(m) *= scalar; return *this; }

    Vector4 Matrix4::operator*(const Vector4& v) const 
    {
        glm::vec4 gv(v.x, v.y, v.z, v.w);
        glm::vec4 res = AS_CONST_GLM(m) * gv;
        return { res.x, res.y, res.z, res.w };
    }

    bool Matrix4::operator==(const Matrix4& other) const 
    {
        for (int i = 0; i < 16; ++i) { if (!Math::Approximately(m[i], other.m[i])) return false; }
        return true;
    }

} // namespace pgn