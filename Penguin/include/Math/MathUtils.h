#pragma once
#include "pgpch.h"

namespace pgn
{
    namespace Math
    {
        const float PI = 3.1415926535f;
        const float E = 2.71828f;
        const float EPSILON = 0.00001f;

        template<typename T>
        bool Approximately(T a, T b) { return std::abs(a - b) < EPSILON; }

        template<typename T>
        T Max(T a, T b) { return (a > b) ? a : b; }

        template<typename T>
        T Min(T a, T b) { return (a < b) ? a : b; }

        template<typename T>
        int Sign(T val) { return (T(0) < val) - (val < T(0)); }

        template<typename T>
        T Abs(T val) { return std::abs(val); }

        template<typename T>
        T Round(T value, int decimals)
        {
            static_assert(std::is_floating_point_v<T>, "Round requires a floating point type.");
            
            // Quick lookup for common decimal places (0-9)
            static const double lookup[] = { 1.0, 10.0, 100.0, 1000.0, 10000.0, 100000.0, 
                                            1000000.0, 10000000.0, 100000000.0, 1000000000.0 };
            
            double factor = (decimals >= 0 && decimals <= 9) ? lookup[decimals] : std::pow(10.0, decimals);
            
            return static_cast<T>(std::round(value * factor) / factor);
        }

        //(Linear Interpolation): Find a value between A and B based on a percentage (0 to 1).
        template<typename T>
        T Lerp(T a, T b, T t) { return a + t * (b - a); }

        //Take a value from one range (e.g., a controller trigger 0 to 255) and map it to another 
        //(e.g., vehicle speed 0.0 to 100.0).
        template<typename T>
        T Remap(T value, T iMin, T iMax, T oMin, T oMax) { return oMin + (value - iMin) * (oMax - oMin) / (iMax - iMin); }

        template<typename T>
        T Clamp(T value, T min, T max) 
        {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        template<typename T>
        T ToRadians(T degrees) { return degrees * (static_cast<T>(PI) / 180); }

        template<typename T>
        T ToDegrees(T radians) { return radians * 180 / (static_cast<T>(PI)); }

        inline float Sin(float angle) { return std::sin(angle); }
        inline float Cos(float angle) { return std::cos(angle); }

    } // namespace Math
} // namespace pgn