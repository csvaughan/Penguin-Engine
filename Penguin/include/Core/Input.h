#pragma once
#include "pgpch.h"
#include "Math/Vector.h"
#include "Events/InputEvents.h"

namespace pgn {

    class Event;

    struct KeyBinding 
    {
        KeyCode MainKey;
        bool Shift = false;
        bool Ctrl = false;
        bool Alt = false;
    };

    struct MouseSettings 
    {
        float Sensitivity = 1.0f;
        float Deadzone = 0.05f; // Ignore movements smaller than this
    };

    class Input 
    {
    public:
        static void LoadConfig(const std::string& filepath);
        static void clearConfig();
        
        // Action Polling (Aliased)
        static bool GetAction(const std::string& alias);
        
        // Raw Polling
        static bool GetKey(KeyCode key) { return s_CurrentState[key]; }
        static bool GetKeyDown(KeyCode key) { return s_CurrentState[key] && !s_LastState[key]; }
        static bool GetKeyUp(KeyCode key) { return !s_CurrentState[key] && s_LastState[key]; }
        static Vector2 GetMousePos() { return s_MousePos; }
        
        static float getMouseX() { return getMouseMovement().x; }
        static float getMouseY() { return getMouseMovement().y; }
        static Vector2 getMouseMovement();

        // Mouse Settings
        static void SetMouseSensitivity(float sensitivity) { s_MouseSettings.Sensitivity = sensitivity; }
        static void SetMouseDeadzone(float deadzone) { s_MouseSettings.Deadzone = deadzone; }
        static float GetMouseSensitivity() { return s_MouseSettings.Sensitivity; }
        
    private:
        static void OnEvent(Event& e);
        static void OnUpdate();
        static void ClearStates() 
        { s_CurrentState.clear(); s_LastState.clear(); }

    private:
        //Keyboard
        static std::unordered_map<std::string, KeyBinding> s_Bindings;
        static std::unordered_map<KeyCode, bool> s_CurrentState;
        static std::unordered_map<KeyCode, bool> s_LastState;
        static std::unordered_map<std::string, KeyCode> s_StringToKeyMap;

        //Mouse 
        static Vector2 s_MousePos;
        static Vector2 s_MouseDelta;
        static MouseSettings s_MouseSettings;

        friend class Application;
    };
}