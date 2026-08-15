#include "GUI/Slider.h"
#include "GUI/UICanvas.h"
#include "Events/InputEvents.h"
#include "Renderer/Renderer.h"
#include "Math/MathUtils.h"
#include <cmath>
#include <utility>

namespace pgn::GUI {

    Slider::Slider(UICanvas* canvas, UIElementID id, Vector2 pos, Vector2 size, UILayoutDirection direction, const std::string& name, GUIElement* parent)
        : Panel(canvas, id, pos, size, name, parent), m_orientation(direction) // Match Panel base class parameters
    {
        SetBackgroundColor(Color{ 40, 40, 40, 255 });
        
        // Ensure size coordinates match initial orientation layout rules
        AdjustBoundsForOrientation();
        UpdateHandlePosition();
    }

    void Slider::SetRange(float min, float max) 
    { 
        m_minValue = min; 
        m_maxValue = max; 
        
        // Re-validate the step size against the new range boundaries
        if (m_stepSize > 0.0f)
            SetStepSize(m_stepSize); 
    }

    void Slider::SetStepSize(float size)
    {
        // 1. Enforce positive values. If a developer passes -5.0, they mean a step of 5.0.
        float cleanSize = std::abs(size);

        // 2. Prevent a step size larger than the usable track range
        float totalRange = m_maxValue - m_minValue;
        if (cleanSize > totalRange && totalRange > 0.0f)
        {
            cleanSize = totalRange;
        }

        m_stepSize = cleanSize;

        // 3. Immediately snap the current value to the new step constraints
        SetValue(GetValue());
    }

    void Slider::SetOrientation(UILayoutDirection orientation)
    {
        m_orientation = orientation;
        AdjustBoundsForOrientation();
        UpdateHandlePosition();
    }

    void Slider::SetValue(float value)
    {
        float clampedValue = Math::Clamp(value, m_minValue, m_maxValue);

        // Snap using the actual value domain instead of normalized 0-1 space
        if (m_stepSize > 0.0f)
        {
            clampedValue = m_minValue + std::round((clampedValue - m_minValue) / m_stepSize) * m_stepSize;
            clampedValue = Math::Clamp(clampedValue, m_minValue, m_maxValue); // Ensure rounding didn't push it out of bounds
        }

        float normalized = (clampedValue - m_minValue) / (m_maxValue - m_minValue);

        if (m_value != normalized)
        {
            m_value = normalized;
            UpdateHandlePosition();
            if (m_onValueChanged) m_onValueChanged(GetValue());
        }
    }

    void Slider::UpdateHandlePosition()
    {
        FloatRect bounds = GetGlobalBounds();
        bool isHoriz = (m_orientation == UILayoutDirection::Horizontal);
        float scale = m_canvas->GetScaleFactor();

        float scaledThickness = m_handleThickness * scale;
        float scaledOverhang = m_handleOverhang * scale;
        float halfThickness = scaledThickness * 0.5f;
        float halfOverhang = scaledOverhang * 0.5f;

        // 1. Map to screen bounds and pixel-snap the starting positions
        float mainStart  = std::floor(isHoriz ? bounds.x : bounds.y);
        float mainSize   = std::ceil(isHoriz ? bounds.w : bounds.h);
        float crossStart = std::floor(isHoriz ? bounds.y : bounds.x);
        float crossSize  = std::ceil(isHoriz ? bounds.h : bounds.w);

        // 2. Position and snap handle
        float handleMain  = std::floor(mainStart + (isHoriz ? m_value : (1.0f - m_value)) * mainSize - halfThickness);
        float handleCross = std::floor(crossStart - halfOverhang); 

        m_handle.setPosition(isHoriz ? Vector2{handleMain, handleCross} : Vector2{handleCross, handleMain});
        m_handle.setSize(isHoriz ? Vector2{scaledThickness, crossSize + scaledOverhang} : Vector2{crossSize + scaledOverhang, scaledThickness});

        // 3. Position and snap fill bar
        if (m_hasFill)
        {
            // Use std::ceil to ensure the fill bar seamlessly meets the handle without sub-pixel gaps
            float fillSizeMain = std::ceil(m_value * mainSize);
            float fillPosMain  = std::floor(isHoriz ? mainStart : (mainStart + mainSize - fillSizeMain));

            m_fillBar.setPosition(isHoriz ? Vector2{fillPosMain, crossStart} : Vector2{crossStart, fillPosMain});
            m_fillBar.setSize(isHoriz ? Vector2{fillSizeMain, crossSize} : Vector2{crossSize, fillSizeMain});
        }
    }

    void Slider::UpdateValueFromMouse(Vector2 mousePos)
    {
        FloatRect bounds = GetGlobalBounds();
        bool isHoriz = (m_orientation == UILayoutDirection::Horizontal);

        float trackSize = isHoriz ? bounds.w : bounds.h;
        if (trackSize <= 0.0f) return;

        // Calculate percentage along the main axis
        float pct = isHoriz ? (mousePos.x - bounds.x) / trackSize : 1.0f - ((mousePos.y - bounds.y) / trackSize);

        float targetValue = m_minValue + Math::Clamp(pct, 0.0f, 1.0f) * (m_maxValue - m_minValue);
        SetValue(targetValue);
    }

    void Slider::AdjustBoundsForOrientation()
    {
        // If it's a Vertical slider but wide, swap dimensions to make it tall
        if (m_orientation == UILayoutDirection::Vertical && m_bounds.w > m_bounds.h)
        {
            std::swap(m_bounds.w, m_bounds.h);
        }
        // If it's a Horizontal slider but tall, swap dimensions to make it wide
        else if (m_orientation == UILayoutDirection::Horizontal && m_bounds.h > m_bounds.w)
        {
            std::swap(m_bounds.w, m_bounds.h);
        }
    }

    void Slider::OnUpdate(Timestep ts)
    {
        if (!m_enabled) return;
        Panel::OnUpdate(ts);

        if (m_dirty) 
            UpdateHandlePosition();
    }

    void Slider::OnRender(float alpha, Renderer& renderer)
    {
        if (!m_visible) return;

        // 1. Renders Background (Z) and Optional Image (Z + 1)
        Panel::OnRender(alpha, renderer);

        // 2. Safely layer the Slider components above the Panel's components
        m_fillBar.setZIndex(m_zIndex + 2);
        m_handle.setZIndex(m_zIndex + 3);

        // 3. Render Fill Bar (only if it has volume to render)
        if (m_hasFill && m_value > 0.0f)
        {
            m_fillBar.setColor(m_fillColor);
            m_fillBar.render(renderer);
        }

        // 4. Render Handle
        m_handle.setColor(m_handleColor);
        m_handle.render(renderer);
    }

    bool Slider::OnEvent(Event& e)
    {
        if (!m_visible || !m_enabled) return false;
        if (e.Handled) return true; 

        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<MouseButtonPressedEvent>([&](MouseButtonPressedEvent& ev) {
            if (Contains(ev.GetPosition()))
            {
                m_isDragging = true;
                UpdateValueFromMouse(ev.GetPosition());
                return true; 
            }
            return false;
        });

        dispatcher.Dispatch<MouseMovedEvent>([&](MouseMovedEvent& ev) {
            if (m_isDragging)
            {
                UpdateValueFromMouse(ev.GetPosition());
                return true;
            }
            return false;
        });

        dispatcher.Dispatch<MouseButtonReleasedEvent>([&](MouseButtonReleasedEvent& ev) {
            if (m_isDragging)
            {
                m_isDragging = false;
                return true;
            }
            return false;
        });

        if (e.Handled) return true;
        return Panel::OnEvent(e);
    }
}