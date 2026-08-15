#pragma once
#include "Renderable.h"
#include "VertexArray.h"
#include "Renderer.h"

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

        float getWidth() {return m_size.x; }
        float getHeight() {return m_size.y; }

        FloatRect getLocalBounds() const override { return { 0.0f, 0.0f, m_size.x, m_size.y }; }

        void render(Renderer& renderer) const override
        {
            const auto& t = getTransform();
            const Vector2 size = getSize();
            const Color color = getColor();

            VertexArray va;

            auto addQuad = [&](Vector2 p, Vector2 s) {
                int base = static_cast<int>(va.getVertices().size());
                va.addIndex(base);     va.addIndex(base + 1); va.addIndex(base + 2);
                va.addIndex(base + 2); va.addIndex(base + 3); va.addIndex(base);

                // Subtract origin in local space
                va.addVertex({ p - t.origin, color, {0.0f, 0.0f} });
                va.addVertex({ {p.x + s.x - t.origin.x, p.y - t.origin.y}, color, {0.0f, 0.0f} });
                va.addVertex({ p + s - t.origin, color, {0.0f, 0.0f} });
                va.addVertex({ {p.x - t.origin.x, p.y + s.y - t.origin.y}, color, {0.0f, 0.0f} });
            };

            if (isFilled()) 
            {
                addQuad({0.0f, 0.0f}, size);
            }
            else 
            {
                float th = getOutlineThickness();
                addQuad({0.0f, 0.0f}, {size.x, th});
                addQuad({0.0f, size.y - th}, {size.x, th});
                addQuad({0.0f, th}, {th, size.y - 2.0f * th});
                addQuad({size.x - th, th}, {th, size.y - 2.0f * th});
            }

            renderer.Submit(va, nullptr, t.GetModelMatrix(), getZIndex(), RenderPass::Diffuse);
        }

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

        void render(Renderer& renderer) const override
        {
            const auto& t = getTransform();
            const float radius = getRadius();
            const Color color = getColor();
            const int segments = 64;

            VertexArray va;

            if (isFilled())
            {
                va.addVertex({t.origin, color, {0.5f, 0.5f} });

                for (int i = 0; i <= segments; ++i)
                {
                    float theta = i * 2.0f * Math::PI / segments;
                    Vector2 localPos = Vector2{ radius * Math::Cos(theta), radius * Math::Sin(theta) } - t.origin;
                    Vector2 uv = { (Math::Cos(theta) + 1.0f) * 0.5f, (Math::Sin(theta) + 1.0f) * 0.5f };

                    va.addVertex({ localPos, color, uv });

                    if (i < segments) {
                        va.addIndex(0);
                        va.addIndex(i + 1);
                        va.addIndex(i + 2);
                    }
                }
            }
            else
            {
                float thickness = getOutlineThickness();
                for (int i = 0; i <= segments; ++i)
                {
                    float theta = i * 2.0f * Math::PI / segments;
                    float cosT = Math::Cos(theta); 
                    float sinT = Math::Sin(theta);

                    Vector2 outerP = Vector2{ radius * cosT, radius * sinT } - t.origin;
                    Vector2 innerP = Vector2{ (radius - thickness) * cosT, (radius - thickness) * sinT } - t.origin;

                    va.addVertex({ innerP, color, {0.0f, 0.0f} });
                    va.addVertex({ outerP, color, {0.0f, 0.0f} });

                    if (i < segments) {
                        int base = i * 2;
                        va.addIndex(base);     va.addIndex(base + 1); va.addIndex(base + 2);
                        va.addIndex(base + 1); va.addIndex(base + 3); va.addIndex(base + 2);
                    }
                }
            }

            renderer.Submit(va, nullptr, t.GetModelMatrix(), getZIndex(), RenderPass::Diffuse);
        }
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

        void setPoints(Vector2 p1, Vector2 p2) 
        {
            Vector2 center = (p1 + p2) * 0.5f;
            setPosition(center); 
            
            m_localP1 = p1 - center; // Offset from center to p1
            m_localP2 = p2 - center; // Offset from center to p2
        }

        void render(Renderer& renderer) const override
        {
            const auto& t = getTransform();
            const Color color = getColor();

            Vector2 p1 = getPoint1() - t.origin;
            Vector2 p2 = getPoint2() - t.origin;

            Vector2 dir = p2 - p1;
            if (dir.LengthSquared() < 0.001f) return;

            Vector2 normal = Vector2{ -dir.y, dir.x }.Normalize();
            float halfThickness = Math::Max(getOutlineThickness(), 1.0f) * 0.5f;
            Vector2 offset = normal * halfThickness;

            VertexArray va;
            va.addIndex(0); va.addIndex(1); va.addIndex(2);
            va.addIndex(2); va.addIndex(3); va.addIndex(0);

            va.addVertex({ p1 + offset, color, {0.0f, 0.0f} });
            va.addVertex({ p2 + offset, color, {0.0f, 0.0f} });
            va.addVertex({ p2 - offset, color, {0.0f, 0.0f} });
            va.addVertex({ p1 - offset, color, {0.0f, 0.0f} });

            renderer.Submit(va, nullptr, t.GetModelMatrix(), getZIndex(), RenderPass::Diffuse);
        }

    private:

        Vector2 m_localP1;
        Vector2 m_localP2;
    };
}