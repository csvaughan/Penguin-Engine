#pragma once
#include "Math/Rect.h"
#include "Renderable.h"

namespace pgn {

    class RenderShape : public Renderable
    {
    public:
        virtual ~RenderShape() = default;

        RenderShape(Vector2 pos = {0,0}, Color color = Color::White)
        {
            setPosition(pos);
            setScale({1.0f, 1.0f});
            setColor(color);
        }
        
        float getOutlineThickness() const { return m_thickness; }
        void setOutlineThickness(float thickness) 
        { 
            m_thickness = thickness; 
            m_isFilled = (thickness == 0); 
        }

        bool isFilled() const { return m_isFilled; }

    protected:
        float m_thickness = 0.0f; // 0 means filled
        bool m_isFilled = true;
    };

    class RectangleShape : public RenderShape 
    {
    public:
        RectangleShape(Vector2 pos = {0,0}, Vector2 size = {100,100}, Color color = Color::White) : RenderShape(pos, color), m_size(size) {}
        
        void setSize(Vector2 size) { m_size = size; }
        Vector2 getSize() const    { return m_size; }

        FloatRect getLocalBounds() const override { return { 0.0f, 0.0f, m_size.x, m_size.y }; }

    private:
        Vector2 m_size;
    };

    class CircleShape : public RenderShape 
    {
    public:
        CircleShape(Vector2 pos = {0,0}, float radius = 50.0f, Color color = Color::White) : RenderShape(pos, color), m_radius(radius) {}

        void setRadius(float r) { m_radius = r; }
        float getRadius() const { return m_radius; }
        FloatRect getLocalBounds() const override { return { -m_radius, -m_radius, m_radius * 2.0f, m_radius * 2.0f }; }
    private:
        float m_radius = 50.0f;
    };

    class LineShape : public RenderShape 
    {
    public:
        LineShape(Vector2 p1 = {0, 0}, Vector2 p2 = {1, 1}, Color color = Color::White) : RenderShape(p1, color) {}

        void setPoint1(Vector2 p1) { setPoints(p1, getPoint2World()); }
        void setPoint2(Vector2 p2) { setPoints(getPoint1World(), p2); }

        // Returns local coordinates for the Renderer
        Vector2 getPoint1() const { return m_localP1; }
        Vector2 getPoint2() const { return m_localP2; }

        // Helpers to get where the points actually are in the world
        Vector2 getPoint1World() const { return getPosition() + m_localP1; }
        Vector2 getPoint2World() const { return getPosition() + m_localP2; }

        FloatRect getLocalBounds() const override 
        { 
            float minX = std::min(m_localP1.x, m_localP2.x);
            float minY = std::min(m_localP1.y, m_localP2.y);
            return { minX, minY, std::abs(m_localP1.x - m_localP2.x), std::abs(m_localP1.y - m_localP2.y) };
        }

        FloatRect getGlobalBounds() const override 
        {
            FloatRect local = getLocalBounds();
            return { getPosition().x + local.x, getPosition().y + local.y, local.w, local.h };
        }

    private:
        void setPoints(Vector2 p1, Vector2 p2) 
        {
            Vector2 center = (p1 + p2) * 0.5f;
            setPosition(center); 
            
            m_localP1 = p1 - center; // Offset from center to p1
            m_localP2 = p2 - center; // Offset from center to p2
        }

        Vector2 m_localP1;
        Vector2 m_localP2;
    };
}