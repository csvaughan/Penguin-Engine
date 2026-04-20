#pragma once
#include "pgpch.h"

namespace pgn
{
    class Color 
    {
    public:
        float r, g, b, a;

        Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) 
            : r(r), g(g), b(b), a(a) {}

        static Color FromBytes(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        {
            return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        }

        static Color FromHex(std::string_view hex)
        {
            if (hex.starts_with('#')) hex.remove_prefix(1);
            else if (hex.starts_with("0x")) hex.remove_prefix(2);

            uint32_t value = 0;
            // from_chars returns a 'ptr' and 'ec' (error code)
            auto [ptr, ec] = std::from_chars(hex.data(), hex.data() + hex.size(), value, 16);

            // If parsing failed, return Black (or a fallback color)
            if (ec != std::errc()) { return Color::Magenta; }

            if (hex.length() <= 6)
            {
                return FromBytes(
                    (value >> 16) & 0xFF,
                    (value >> 8) & 0xFF,
                    (value) & 0xFF,
                    255
                );
            }
            else
            {
                return FromBytes(
                    (value >> 24) & 0xFF,
                    (value >> 16) & 0xFF,
                    (value >> 8) & 0xFF,
                    (value) & 0xFF
                );
            }
        }

        uint32_t ToUint32() const
        {
            uint8_t r8 = static_cast<uint8_t>(r * 255.0f);
            uint8_t g8 = static_cast<uint8_t>(g * 255.0f);
            uint8_t b8 = static_cast<uint8_t>(b * 255.0f);
            uint8_t a8 = static_cast<uint8_t>(a * 255.0f);

            return (r8 << 24) | (g8 << 16) | (b8 << 8) | a8;
        }

        /**
         * Returns a string representation: "Color(R: 1.00, G: 0.50, B: 0.00, A: 1.00)"
         */
        std::string ToString() const
        {
            return std::format("Color(R: {:.2f}, G: {:.2f}, B: {:.2f}, A: {:.2f})", r, g, b, a);
        }

        /**
         * Returns the Hex string representation (e.g., "#FFA500FF")
         */
        std::string ToHexString() const
        {
            uint8_t r8 = static_cast<uint8_t>(r * 255.0f);
            uint8_t g8 = static_cast<uint8_t>(g * 255.0f);
            uint8_t b8 = static_cast<uint8_t>(b * 255.0f);
            uint8_t a8 = static_cast<uint8_t>(a * 255.0f);

            return std::format("#{:02X}{:02X}{:02X}{:02X}", r8, g8, b8, a8);
        }

        // --- Math Operators ---
        bool operator==(const Color& other) const { return (r == other.r && g == other.g && b == other.b && a == other.a); }

        Color operator+(const Color& other) const 
        {
            return Color(
                std::min(r + other.r, 1.0f),
                std::min(g + other.g, 1.0f),
                std::min(b + other.b, 1.0f),
                std::min(a + other.a, 1.0f)
            );
        }

        Color operator*(const Color& other) const { return Color(r * other.r, g * other.g, b * other.b, a * other.a); }

        // Multiply color by a float (e.g., Color::Green * 0.5f)
        Color operator*(float scalar) const
        {
            return Color(
                std::clamp(r * scalar, 0.0f, 1.0f),
                std::clamp(g * scalar, 0.0f, 1.0f),
                std::clamp(b * scalar, 0.0f, 1.0f),
                std::clamp(a * scalar, 0.0f, 1.0f)
            );
        }

        // Support for scalar * Color (e.g., 0.5f * Color::Green)
        friend Color operator*(float scalar, const Color& color) { return color * scalar; }

        // Linear Interpolation
        static Color Lerp(const Color& a, const Color& b, float t)
        {
            t = std::clamp(t, 0.0f, 1.0f);
            return Color(
                a.r + (b.r - a.r) * t,
                a.g + (b.g - a.g) * t,
                a.b + (b.b - a.b) * t,
                a.a + (b.a - a.a) * t
            );
        }

         // --- Constants ---
        static const Color Black;
        static const Color White;
        static const Color Grey;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Magenta;
        static const Color Cyan;
        static const Color Yellow;
        static const Color Transparent;
    };

    inline const Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);
    inline const Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);
    inline const Color Color::Grey(0.5f, 0.5f, 0.5f, 1.0f);
    inline const Color Color::Red(1.0f, 0.0f, 0.0f, 1.0f);
    inline const Color Color::Green(0.0f, 1.0f, 0.0f, 1.0f);
    inline const Color Color::Blue(0.0f, 0.0f, 1.0f, 1.0f);
    inline const Color Color::Magenta(1.0f, 0.0f, 1.0f, 1.0f);
    inline const Color Color::Cyan(0.0f, 1.0f, 1.0f, 1.0f);
    inline const Color Color::Yellow(1.0f, 1.0f, 0.0f, 1.0f);
    inline const Color Color::Transparent(0.0f, 0.0f, 0.0f, 0.0f);
} // namespace pgn