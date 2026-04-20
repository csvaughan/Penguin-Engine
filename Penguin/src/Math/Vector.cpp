#include "Math/Vector.h"
#include "Math/MathUtils.h"

namespace pgn
{

    // Vector 4
    std::ostream &operator<<(std::ostream &os, const Vector4 &v)
    {
        return os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w <<")"; 
    }

    bool Vector4::operator==(const Vector4 &v) const
    { 
        return Math::Approximately(x, v.x) && Math::Approximately(y, v.y) && 
                Math::Approximately(z, v.z) && Math::Approximately(w, v.w); 
    }

    // Vector 3
    std::ostream &operator<<(std::ostream &os, const Vector3 &v)
    {
        return os << "(" << v.x << ", " << v.y << ", " << v.z << ")"; 
    }

    float Vector3::Length() const{ return std::sqrt(LengthSquared()); }

    bool Vector3::operator==(const Vector3 &v) const
    { 
        return Math::Approximately(x, v.x) && Math::Approximately(y, v.y) && Math::Approximately(z, v.z); 
    }

    // Vector 2
    std::ostream &operator<<(std::ostream &os, const Vector2 &v)
    {
        return os << "(" << v.x << ", " << v.y << ")"; 
    }

    bool Vector2::operator==(const Vector2 &v) const { return Math::Approximately(x, v.x) && Math::Approximately(y, v.y); }

    float Vector2::Length() const{ return std::sqrt(LengthSquared()); }

} // namespace pgn