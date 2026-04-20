#include "pgpch.h"
#include "Renderer/CameraController.h"
#include "Math/MathUtils.h"

namespace pgn {

    CameraController::CameraController(float width, float height)
        : m_camera(width, height), m_viewportWidth(width), m_viewportHeight(height)
    {
            // Set target to center of screen (or wherever you want world start to be)
            m_targetPosition = Vector2(width / 2.0f, height / 2.0f);
    
            // SNAP the actual position to target so we don't drift on startup
            m_actualPosition = m_targetPosition; 
    
            // Sync the internal camera immediately
            m_camera.setPosition(m_actualPosition);
    }

    void CameraController::onUpdate(float dt)
    {
        if (m_target) 
        {
            // 1. Deadzone Logic
            // Calculate the distance between the camera target and the player
            Vector2 diff = *m_target - m_targetPosition;

            // If the player moves beyond the horizontal deadzone, push the target
            if (std::abs(diff.x) > m_deadzoneSize.x) 
                m_targetPosition.x = m_target->x - (diff.x > 0 ? m_deadzoneSize.x : -m_deadzoneSize.x);

            // If the player moves beyond the vertical deadzone, push the target
            if (std::abs(diff.y) > m_deadzoneSize.y) 
                m_targetPosition.y = m_target->y - (diff.y > 0 ? m_deadzoneSize.y : -m_deadzoneSize.y);
        }

        float posT = 1.0f - powf(1.0f - m_lerpSpeed, dt * 60.0f);
        m_actualPosition.x = Math::Lerp(m_actualPosition.x, m_targetPosition.x, posT);
        m_actualPosition.y = Math::Lerp(m_actualPosition.y, m_targetPosition.y, posT);

        float zoomT = 1.0f - powf(1.0f - 0.1f, dt * 60.0f);
        m_actualZoom = Math::Lerp(m_actualZoom, m_targetZoom, zoomT);

        if (m_useBounds) 
        {
            float halfWidth = (m_viewportWidth * 0.5f) / m_actualZoom;
            float halfHeight = (m_viewportHeight * 0.5f) / m_actualZoom;
            float levelWidth = m_maxBounds.x - m_minBounds.x;
            float levelHeight = m_maxBounds.y - m_minBounds.y;

            // --- X Axis ---
            m_actualPosition.x = (halfWidth * 2.0f > levelWidth) // View is wider than level: Center it
                ? m_minBounds.x + levelWidth * 0.5f : std::clamp(m_actualPosition.x, m_minBounds.x + halfWidth, m_maxBounds.x - halfWidth);
                
            // --- Y Axis ---
            m_actualPosition.y = (halfHeight * 2.0f > levelHeight)  // View is taller than level: Center it
                ? m_minBounds.y + levelHeight * 0.5f :std::clamp(m_actualPosition.y, m_minBounds.y + halfHeight, m_maxBounds.y - halfHeight);
        }

        m_camera.setPosition(m_actualPosition);
        m_camera.setZoom(m_actualZoom);
    }

    void CameraController::onResize(float width, float height)
    {
        m_viewportWidth = width;
        m_viewportHeight = height;
        m_camera.setViewportSize(width, height);
    }

    void CameraController::move(const Vector2& offset)
    {
        m_targetPosition += offset * m_movementSpeed;
    }

    void CameraController::zoom(float delta)
    {
        m_targetZoom += delta;
        if (m_targetZoom < 0.1f) m_targetZoom = 0.1f;
        if (m_targetZoom > 10.0f) m_targetZoom = 10.0f;
    }

    Vector2 CameraController::screenToWorld(const Vector2& screenPos)
    {   
        // 1. Create the 4D vector from the screen pixels
        Vector4 screenVec = Vector4(screenPos.x, screenPos.y, 0.0f, 1.0f);
        
        // 2. Multiply by the inverse matrix
        Vector4 worldSpaceVec = m_camera.getInverseViewProjection() * screenVec;
        return Vector2(worldSpaceVec.x, worldSpaceVec.y);
    }
}