#pragma once
#include "Event.h"

namespace pgn
{
    //
    // Key Events
    //

    enum class KeyCode : int 
    {
        #define KEY_DEF(name, code, str) name = code,
        #include "Core/KeyList.h"
        #undef KEY_DEF
    };

    class KeyEvent : public Event
    {
    public:
        inline KeyCode GetKeyCode() const { return (KeyCode)m_KeyCode;}
        inline int GetKeyCodeInt() const { return m_KeyCode; }

        // All KeyEvents belong to Keyboard and Input categories
        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

    protected:
        KeyEvent(int keycode) : m_KeyCode(keycode) {}
        int m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(int keycode, bool isRepeat) : KeyEvent(keycode), m_IsRepeat(isRepeat) {}

        inline bool IsRepeat() const { return m_IsRepeat; }

        std::string ToString() const override
        {
            return std::format("KeyPressedEvent: {} (repeat={})", m_KeyCode, m_IsRepeat);
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        bool m_IsRepeat;
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

        std::string ToString() const override
        {
            return std::format("KeyReleasedEvent: {}", m_KeyCode);
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    //
    // Mouse Events
    //

    class MouseEvent : public Event
    {
    public:
        inline Vector2 GetPosition() const { return m_Pos; }
        inline float GetX() const { return m_Pos.x; }
        inline float GetY() const { return m_Pos.y; }

        // All MouseEvents belong to Mouse and Input categories
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    protected:
        MouseEvent(Vector2 pos) : m_Pos(pos) {}
        Vector2 m_Pos;
    };

    class MouseMovedEvent : public MouseEvent
    {
    public:
        MouseMovedEvent(Vector2 pos) : MouseEvent(pos) {}

        std::string ToString() const override
        {
            return std::format("MouseMovedEvent: {}, {}", m_Pos.x, m_Pos.y);
        }

        EVENT_CLASS_TYPE(MouseMoved)
    };

    class MouseScrolledEvent : public MouseEvent
    {
    public:
        MouseScrolledEvent(Vector2 pos, float xOffset, float yOffset) : MouseEvent(pos), m_XOffset(xOffset), m_YOffset(yOffset) {}

        inline float GetXOffset() const { return m_XOffset; }
        inline float GetYOffset() const { return m_YOffset; }

        std::string ToString() const override
        {
            return std::format("MouseScrolledEvent: at ({}, {}), offset ({}, {})", m_Pos.x, m_Pos.y, m_XOffset, m_YOffset);
        }

        EVENT_CLASS_TYPE(MouseScrolled)
    private:
        float m_XOffset, m_YOffset;
    };

    class MouseButtonEvent : public MouseEvent
    {
    public:
        inline int GetMouseButton() const { return m_Button; }

        // Button events add the MouseButton specific flag
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

    protected:
        MouseButtonEvent(int button, Vector2 pos) : MouseEvent(pos), m_Button(button) {}
        int m_Button;
    };

    class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent(int button, Vector2 pos) : MouseButtonEvent(button, pos) {}

        std::string ToString() const override
        {
            return std::format("MouseButtonPressedEvent: {} at ({}, {})", m_Button, m_Pos.x, m_Pos.y);
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent(int button, Vector2 pos) : MouseButtonEvent(button, pos) {}

        std::string ToString() const override
        {
            return std::format("MouseButtonReleasedEvent: {} at ({}, {})", m_Button, m_Pos.x, m_Pos.y);
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };
}