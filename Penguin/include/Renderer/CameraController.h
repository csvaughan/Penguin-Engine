#pragma once
#include "Renderer/Camera.h"

namespace pgn {

    class CameraController 
    {
    public:
        CameraController(float width, float height);

        void onUpdate(float dt);
        void onResize(float width, float height);
        
        // Input Helpers
        void move(const Vector2& offset);
        void zoom(float delta); // Use for Mouse Wheel (delta usually 0.1 or -0.1)

        // World/Screen conversion
        Vector2 screenToWorld(const Vector2& screenPos);

        // Target Following
        void setTarget(const Vector2* target)   { m_target = target; }
        void clearTarget()                      { m_target = nullptr; }
        void setLerpSpeed(float speed)          { m_lerpSpeed = speed; }
        void setDeadzoneSize(const Vector2& dz) {m_deadzoneSize = dz; }
        void toggleUseBounds(bool tf)           { m_useBounds = tf; }
        void setMovementSpeed(float s)          {m_movementSpeed = s; }
        void setBounds(const Vector2& min, const Vector2& max)  { m_minBounds = min; m_maxBounds = max;}
        void setViewportSize(float w, float h) {m_camera.setViewportSize(w, h); m_viewportWidth = w; m_viewportHeight = h;}

        // Getters
        float getViewportWidth() const { return m_viewportWidth; }
        float getViewportHeight() const { return m_viewportHeight; }
        Vector2 getTargetPosition() const { return m_targetPosition; }
        Vector2 getDeadzoneSize() const { return m_deadzoneSize; }
        Camera& getCamera() { return m_camera; }
        const FloatRect getViewport() const { return m_camera.getViewportBounds();}
        const Camera& getCamera() const { return m_camera; }
        const float getMovementSpeed() const {return m_movementSpeed;}

    private:
        Camera m_camera;
        
        // Smoothing variables
        Vector2 m_actualPosition{ 0.0f, 0.0f };
        float m_actualZoom = 1.0f;
        
        // Target values (what we want to reach)
        Vector2 m_targetPosition{ 0.0f, 0.0f };
        float m_targetZoom = 1.0f;

        // Configuration
        const Vector2* m_target = nullptr;
        float m_lerpSpeed = 0.125f; // Position smoothing
        float m_zoomSpeed = 10.0f;  // Zoom smoothing
        Vector2 m_deadzoneSize{ 100.0f, 100.0f }; // Width and Height of the box
        float m_movementSpeed = 500.0f;

        //Bounds
        bool m_useBounds = false;
        Vector2 m_minBounds; // Top-Left of level (e.g. 0, 0)
        Vector2 m_maxBounds; // Bottom-Right of level (e.g. 2000, 2000)
        
        float m_viewportWidth;
        float m_viewportHeight;
    };
}