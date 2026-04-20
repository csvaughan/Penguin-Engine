#pragma once
#include "Color.h"
#include "Math/Transform.h"
#include "Math/Rect.h"
#include "Math/MathUtils.h"

namespace pgn
{
    class Renderable
    {
    public:
        Renderable() = default;
        virtual ~Renderable() = default;
    
        void setPosition(float x, float y)      { m_transform.position = { x, y }; }
        void setPosition(Vector2 pos)           { m_transform.position = pos; }
        void setRotation(float angle)           { m_transform.rotation = angle; }
        void setScale(Vector2 factors)          { m_transform.scale = { Math::Abs(factors.x), Math::Abs(factors.y) }; }
        void setOrigin(Vector2 origin)          { m_transform.origin = origin; }
        void setOrigin(float x, float y)        { m_transform.origin = { x, y }; }
        void setZIndex(int z)                   { m_zIndex = z; }
        virtual void setColor(Color c)          { m_color = c; }

        virtual void setOpacity(float opacity)  { m_color.a = Math::Clamp(opacity, 0.0f, 1.0f);}

        Transform2D& getTransform()             { return m_transform; }
        const Transform2D& getTransform() const { return m_transform; }
        const Vector2 getPosition() const       { return m_transform.position;}
        const Vector2 getOrigin() const         { return m_transform.origin; }
        const Vector2 getScale() const          { return m_transform.scale; }
        float getRotation() const               { return m_transform.rotation;}
        Color getColor() const                  { return m_color; }
        int getZIndex() const                   { return m_zIndex; }
        Vector2 getSize() const                 { return getGlobalBounds().getSize();}

        virtual FloatRect getLocalBounds() const = 0;
        virtual FloatRect getGlobalBounds() const 
        {
            FloatRect local = getLocalBounds();

            // 1. Get the four corners of the local bounding box relative to the origin
            float left = local.x - m_transform.origin.x;
            float top = local.y - m_transform.origin.y;
            float right = left + local.w;
            float bottom = top + local.h;

            Vector2 corners[4] = { { left, top }, { right, top }, { left, bottom }, { right, bottom } };

            // 2. Rotate and scale each corner, then translate to world position
            float angleRad = Math::ToRadians(m_transform.rotation); 
            float cosA = std::cos(angleRad);
            float sinA = std::sin(angleRad);

            float minX = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float minY = std::numeric_limits<float>::max();
            float maxY = std::numeric_limits<float>::lowest();

            for (auto& corner : corners) 
            {
                // Scale
                corner.x *= m_transform.scale.x;
                corner.y *= m_transform.scale.y;

                // Rotate
                float rotatedX = corner.x * cosA - corner.y * sinA;
                float rotatedY = corner.x * sinA + corner.y * cosA;

                // Translate
                float worldX = rotatedX + m_transform.position.x;
                float worldY = rotatedY + m_transform.position.y;

                // Update AABB bounds
                minX = std::min(minX, worldX);
                maxX = std::max(maxX, worldX);
                minY = std::min(minY, worldY);
                maxY = std::max(maxY, worldY);
            }

            return { minX, minY, maxX - minX, maxY - minY };
        }

        void centerOrigin() 
        { 
            FloatRect bounds = getLocalBounds(); 
            setOrigin({ bounds.x + (bounds.w / 2.0f), bounds.y + (bounds.h / 2.0f) }); 
        }

    private:
        Transform2D m_transform;
        Color m_color{Color::White};
        int m_zIndex = 0;
    };
} // namespace pgn