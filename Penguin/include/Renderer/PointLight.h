#pragma once
#include "Renderable.h"
#include "Renderer.h"

namespace pgn {

    class PointLight : public Renderable
    {
    public:
        PointLight(Vector2 position = { 0.0f, 0.0f }, float radius = 150.0f, Color color = Color::White)
            : m_radius(radius)
        {
            setPosition(position);
            setColor(color);
        }

        // --- Light Attributes ---
        void setRadius(float radius) { m_radius = std::max(0.0f, radius); }
        float getRadius() const      { return m_radius; }

        void setIntensity(float intensity) { setOpacity(intensity); }
        float getIntensity() const         { return getColor().a / 255.0f; }

        // Custom mask texture (falls back to procedural radial gradient if null)
        void setTexture(Ref<Texture> texture) { m_texture = texture; }
        WeakRef<Texture> getTexture() const   { return m_texture; }

        // --- Renderable Overrides ---
        FloatRect getLocalBounds() const override 
        { 
            return { -m_radius, -m_radius, m_radius * 2.0f, m_radius * 2.0f }; 
        }

        void render(Renderer& renderer) const override
        {
            VertexArray va;
            Color centerColor = getColor(); 
            centerColor.a = static_cast<uint8_t>(getIntensity() * 255.0f); 

            auto tex = m_texture.lock(); 
            float r = m_radius; 

            if (tex)
            {
                // Custom texture mask (e.g. flashlight cone)
                va.addQuad(
                    Vertex{ {-r, -r}, centerColor, {0.0f, 0.0f} },
                    Vertex{ { r, -r}, centerColor, {1.0f, 0.0f} },
                    Vertex{ { r,  r}, centerColor, {1.0f, 1.0f} },
                    Vertex{ {-r,  r}, centerColor, {0.0f, 1.0f} }
                );
            }
            else
            {
                // Procedural radial gradient (Center bright -> Outer perimeter 0 alpha)
                Color edgeColor = Color(centerColor.r, centerColor.g, centerColor.b, 0);

                const int segments = 32;
                float step = (2.0f * Math::PI) / segments;

                // Center vertex (full intensity)
                va.addVertex(Vertex{ {0.0f, 0.0f}, centerColor, {0.5f, 0.5f} });

                // Outer vertices (fades out completely at radius edge)
                for (int i = 0; i <= segments; ++i)
                {
                    float angle = i * step;
                    float x = std::cos(angle) * r;
                    float y = std::sin(angle) * r;
                    va.addVertex(Vertex{ {x, y}, edgeColor, {0.5f + (x / (2.0f * r)), 0.5f + (y / (2.0f * r))} });
                }

                for (int i = 1; i <= segments; ++i)
                {
                    va.addIndex(0);
                    va.addIndex(i);
                    va.addIndex(i + 1);
                }
            }

            renderer.Submit(va, tex, getTransform().GetModelMatrix(), getZIndex(), RenderPass::Light);
        }

    private:
        float m_radius = 150.0f;
        WeakRef<Texture> m_texture;
    };

} // namespace pgn