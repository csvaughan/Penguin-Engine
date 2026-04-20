#pragma once
#include "Math/Matrix4.h"
#include "Math/Vector.h"    
#include "Math/Rect.h"

namespace pgn {

    class Camera 
    {
    public:
        Camera(float width, float height) : m_viewportWidth(width), m_viewportHeight(height) 
        {
            updateMatrix();
        }

        void setPosition(const Vector2& position) { m_position = position; updateMatrix(); }
        void setRotation(float rotation) { m_rotation = rotation; updateMatrix(); }
        void setZoom(float zoom) { m_zoom = zoom; updateMatrix(); }
        
        void setViewportSize(float width, float height) 
        {
            m_viewportWidth = width;
            m_viewportHeight = height;
            updateMatrix();
        }

        const Vector2& getPosition() const { return m_position; }
        float getRotation() const { return m_rotation; }
        float getZoom() const { return m_zoom; }

        const Matrix4& getViewProjection() const { return m_viewProjection; }
        const Matrix4& getInverseViewProjection() const { return m_inverseViewProjection; }

        FloatRect getViewportBounds() const 
        {
            float width = m_viewportWidth / m_zoom;
            float height = m_viewportHeight / m_zoom; 
            return { m_position.x - (width * 0.5f), m_position.y - (height * 0.5f), width, height };
        }

    private:
        void updateMatrix() 
        {
            // 1. Move the world so the camera position is the origin
            Matrix4 translation = Matrix4::Translate(Vector3(-m_position.x, -m_position.y, 0.0f));
            
            // 2. Rotate the world around that origin
            Matrix4 rotation = Matrix4::Rotate(-m_rotation, Vector3(0.0f, 0.0f, 1.0f));
            
            // 3. Apply Zoom
            Matrix4 scale = Matrix4::Scale(Vector3(m_zoom, m_zoom, 1.0f));

            // 4. Offset the origin to the center of the screen
            Matrix4 screenCenter = Matrix4::Translate(Vector3(m_viewportWidth * 0.5f, m_viewportHeight * 0.5f, 0.0f));

            // Combine: Scale * Rotate * Translate
            m_viewProjection = screenCenter * scale * rotation * translation;
            
            m_inverseViewProjection = m_viewProjection.Invert();
        }

        Vector2 m_position = { 0.0f, 0.0f };
        float m_rotation = 0.0f;
        float m_zoom = 1.0f;
        float m_viewportWidth;
        float m_viewportHeight;

        Matrix4 m_viewProjection;
        Matrix4 m_inverseViewProjection;
    };
}