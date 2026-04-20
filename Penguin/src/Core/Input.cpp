#include "Core/Input.h"
#include "Log/Log.h"
#include "Math/Vector.h"

namespace pgn {

    std::unordered_map<std::string, KeyBinding> Input::s_Bindings;
    std::unordered_map<KeyCode, bool> Input::s_CurrentState;
    std::unordered_map<KeyCode, bool> Input::s_LastState;

    Vector2 Input::s_MouseDelta = {0,0};
    Vector2 Input::s_MousePos = {0, 0};
    MouseSettings Input::s_MouseSettings;

    // Initialize the string map using X-Macro
    std::unordered_map<std::string, KeyCode> Input::s_StringToKeyMap = {
        #define KEY_DEF(name, code, str) { str, KeyCode::name },
        #include "Core/KeyList.h"
        #undef KEY_DEF
    };

    void Input::OnUpdate() 
    { 
        s_LastState = s_CurrentState;
        s_MouseDelta = { 0.0f, 0.0f };
    }

    Vector2 Input::getMouseMovement()
    {
        Vector2 rawMovement = s_MouseDelta * s_MouseSettings.Sensitivity;
        float length = rawMovement.Length();

        if (length < s_MouseSettings.Deadzone)
            return Vector2::Zero();

        float percentageAboveDeadzone = (length - s_MouseSettings.Deadzone) / (1.0f - s_MouseSettings.Deadzone);
        return (rawMovement / length) * percentageAboveDeadzone;
    }

    void Input::LoadConfig(const std::string& filepath) 
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            PGN_CORE_WARN("Warning! Unable to open file: {}", filepath);
            return;
        }

        std::string line;
        while (std::getline(file, line)) 
        {
            // Ignore empty lines or comments
            if (line.empty() || line[0] == '#') continue;

            size_t delim = line.find('=');
            if (delim == std::string::npos) continue;

            std::string alias = line.substr(0, delim);
            std::string bindingStr = line.substr(delim + 1);

            KeyBinding b;
            std::stringstream ss(bindingStr);
            std::string segment;
            while (std::getline(ss, segment, '+')) 
            {
                if      (segment == "LShift") b.Shift = true;
                else if (segment == "LCtrl")  b.Ctrl = true;
                else if (segment == "LAlt")   b.Alt = true;
                else 
                {
                    // Verify the key exists in our X-Macro generated map
                    auto it = s_StringToKeyMap.find(segment);
                    if (it != s_StringToKeyMap.end()) { 
                        b.MainKey = it->second; 
                    }else { 
                        PGN_CORE_ERROR("Input Error: Unknown key {} in config.", segment);
                    }
                }
            }
            s_Bindings[alias] = b;

            //Mouse Settings
            if (alias == "MouseSensitivity") {
                s_MouseSettings.Sensitivity = std::stof(bindingStr);
                continue; 
            }
            if (alias == "MouseDeadzone") {
                s_MouseSettings.Deadzone = std::stof(bindingStr);
                continue;
            }
        }

        PGN_CORE_INFO("Config file loaded sucessfully.");
    }

    void Input::clearConfig()
    {
        s_Bindings.clear(); 
        PGN_CORE_INFO("Input bindings cleared.");
    }

    bool Input::GetAction(const std::string &alias)
    {
        if (s_Bindings.find(alias) == s_Bindings.end()) return false;
        const auto& b = s_Bindings[alias];
        
        bool modsMatch = (GetKey(KeyCode::LShift) == b.Shift) && 
                         (GetKey(KeyCode::LCtrl)  == b.Ctrl)  && 
                         (GetKey(KeyCode::LAlt)   == b.Alt);

        return modsMatch && GetKey(b.MainKey);
    }

    void Input::OnEvent(Event& e) 
    {   
        EventDispatcher dispatcher(e);

        dispatcher.Dispatch<KeyPressedEvent>([](KeyPressedEvent& ev) {
            s_CurrentState[(KeyCode)ev.GetKeyCode()] = true;
            return false;
        });
        dispatcher.Dispatch<KeyReleasedEvent>([](KeyReleasedEvent& ev) {
            s_CurrentState[(KeyCode)ev.GetKeyCode()] = false;
            return false;
        });
        dispatcher.Dispatch<MouseButtonPressedEvent>([](MouseButtonPressedEvent& ev) {
            s_CurrentState[(KeyCode)ev.GetMouseButton()] = true;
            return false;
        });
        dispatcher.Dispatch<MouseButtonReleasedEvent>([](MouseButtonReleasedEvent& ev) {
            s_CurrentState[(KeyCode)ev.GetMouseButton()] = false;
            return false;
        });
        dispatcher.Dispatch<MouseMovedEvent>([](MouseMovedEvent& ev) {
            // Calculate how far we moved since the last frame
            s_MouseDelta = { (float)ev.GetX() - s_MousePos.x, (float)ev.GetY() - s_MousePos.y };
            s_MousePos = { (float)ev.GetX(), (float)ev.GetY() };
            return false;
        });
    }
}