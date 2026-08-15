#pragma once
#include "Panel.h"
#include <functional>

namespace pgn::GUI {

    class Slider : public Panel
    {
    public:
        Slider(
            UICanvas* canvas, 
            UIElementID id, 
            Vector2 pos, 
            Vector2 size, 
            UILayoutDirection direction, 
            const std::string& name = "Slider", 
            GUIElement* parent = nullptr);

        void SetRange(float min, float max);
        void SetValue(float value);
        float GetValue() const { return m_minValue + (m_value * (m_maxValue - m_minValue));}
        float GetPercentage() const { return m_value; }

        void SetCallback(std::function<void(float)> callback) { m_onValueChanged = callback; }

        void SetHandleColor(Color color) { m_handleColor = color; }
        void SetFillColor(Color color)   { m_fillColor = color; m_hasFill = true; }

        // --- Layout & Orientation Options ---
        void SetOrientation(UILayoutDirection orientation);
        void SetStepSize(float size);
        void SetHandleSize(float thickness) { m_handleThickness = thickness; UpdateHandlePosition(); }

    private:
            
        void OnRender(float alpha, Renderer& renderer) override;
        void OnUpdate(Timestep ts) override;
        bool OnEvent(Event& e) override;

        void UpdateValueFromMouse(Vector2 mousePos);
        void UpdateHandlePosition();
        void AdjustBoundsForOrientation();

    private:
        std::function<void(float)> m_onValueChanged;

        float m_handleThickness = 12.0f;
        float m_handleOverhang = 4.0f;

        float m_value = 0.0f; // Normalized internally (0.0f to 1.0f)
        float m_minValue = 0.0f;
        float m_maxValue = 1.0f;

        float m_stepSize = 0.0f; // 0 = continuous layout
        UILayoutDirection m_orientation = UILayoutDirection::Horizontal;

        bool m_isDragging = false;

        RectangleShape m_handle;
        RectangleShape m_fillBar;
        bool m_hasFill = false;
        
        Color m_handleColor{ 200, 200, 200, 255 };
        Color m_fillColor{ 0, 120, 215, 255 };
    };
}