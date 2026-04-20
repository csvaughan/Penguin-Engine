#pragma once
//#include "pgpch.h"
#include "ECS/Components.h"
#include "AnimationState.h"

namespace pgn
{
    class AnimationControllerComponent : public ECS::Component
    {
        friend class AnimationSystem;

    public:
        AnimationControllerComponent() = default;

        // --- Public API for Parameter Control ---
        void setBool(std::string_view name, bool value) { m_Bools[std::string(name)] = value; }
        bool getBool(std::string_view name) const 
        {
            auto it = m_Bools.find(std::string(name));
            return (it != m_Bools.end()) ? it->second : false;
        }

        void setInt(std::string_view name, int value) { m_Ints[std::string(name)] = value; }
        int getInt(std::string_view name) const 
        {
            auto it = m_Ints.find(std::string(name));
            return (it != m_Ints.end()) ? it->second : 0;
        }

        void setFloat(std::string_view name, float value) { m_Floats[std::string(name)] = value; }
        float getFloat(std::string_view name) const 
        {
            auto it = m_Floats.find(std::string(name));
            return (it != m_Floats.end()) ? it->second : 0.0f;
        }
        
        void setTrigger(std::string_view name) { m_ActiveTriggers.emplace(name); }
        bool getTrigger(std::string_view name) const {return m_ActiveTriggers.count(std::string(name));}

        // --- Public API for State Machine Configuration ---
        void addState(std::string_view name, Animation anim) 
        {
            m_States.emplace(name, AnimationState{ anim, {} });
            // If this is the first state, make it the default
            if (m_CurrentStateName.empty()) {setCurrentState(name);}
        }

        void addTransition(std::string_view fromState, std::string_view toState, const std::vector<Condition>& conditions) 
        {
            auto it = m_States.find(std::string(fromState));
            if (it != m_States.end()) {it->second.transitions.emplace_back(Transition{ std::string(toState), conditions });}
        }
    
        Animation* getCurrentAnimation()
        {
            return m_CurrentState ? &m_CurrentState->animation : nullptr;
        }

        const std::string& getCurrentStateName() const { return m_CurrentStateName; }
        
        // Make Transition's condition checker a friend so it can access private parameter maps
        friend bool Transition::AreConditionsMet(const AnimationControllerComponent& controller) const;

        // --- Core Logic ---
    private:
        void setCurrentState(std::string_view name) 
        {
            auto it = m_States.find(std::string(name));
            if (it != m_States.end()) 
            {
                m_CurrentStateName = name;
                m_CurrentState = &it->second;
                m_CurrentState->animation.reset();
            } 
            else 
            {
                m_CurrentStateName = "";
                m_CurrentState = nullptr;
            }
        }

        void update(float deltaTime)
        {
            if (!m_CurrentState) return;

            // 1. Check for a valid transition
            for (const auto& transition : m_CurrentState->transitions) 
            {
                if (transition.AreConditionsMet(*this)) 
                {
                    setCurrentState(transition.targetStateName);
                    break; // Take the first valid transition
                }
            }

            // 2. Update the current animation
            if (m_CurrentState) {m_CurrentState->animation.update(deltaTime);}

            // 3. Clear all triggers at the end of the frame
            m_ActiveTriggers.clear();
        }

    private:
        // --- Parameters ---
        std::unordered_map<std::string, bool> m_Bools;
        std::unordered_map<std::string, int> m_Ints;
        std::unordered_map<std::string, float> m_Floats;
        std::unordered_set<std::string> m_ActiveTriggers;

        // --- State Machine ---
        std::unordered_map<std::string, AnimationState> m_States;
        std::string m_CurrentStateName;
        AnimationState* m_CurrentState = nullptr;
    };


    // --- Implementation of the condition checker ---
    inline bool Transition::AreConditionsMet(const AnimationControllerComponent& controller) const 
    {
        // Check regular parameter conditions
        for (const auto& condition : conditions)
        {
            bool conditionMet = false;
            // Use std::visit to handle the variant type safely
            std::visit([&](auto&& arg) 
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, bool>) {
                    bool paramValue = controller.getBool(condition.parameterName);
                    if (condition.mode == EComparisonMode::Equals) conditionMet = (paramValue == arg);
                    if (condition.mode == EComparisonMode::NotEquals) conditionMet = (paramValue != arg);
                } else if constexpr (std::is_same_v<T, int>) {
                    int paramValue = controller.getInt(condition.parameterName);
                    if (condition.mode == EComparisonMode::Equals) conditionMet = (paramValue == arg);
                    if (condition.mode == EComparisonMode::NotEquals) conditionMet = (paramValue != arg);
                    if (condition.mode == EComparisonMode::GreaterThan) conditionMet = (paramValue > arg);
                    if (condition.mode == EComparisonMode::LessThan) conditionMet = (paramValue < arg);
                    if (condition.mode == EComparisonMode::GreaterThanOrEqual) conditionMet = (paramValue >= arg);
                    if (condition.mode == EComparisonMode::LessThanOrEqual) conditionMet = (paramValue <= arg);
                } else if constexpr (std::is_same_v<T, float>) {
                    float paramValue = controller.getFloat(condition.parameterName);
                    if (condition.mode == EComparisonMode::GreaterThan) conditionMet = (paramValue > arg);
                    if (condition.mode == EComparisonMode::LessThan) conditionMet = (paramValue < arg);
                    // Add other float comparisons if needed
                }
            }, condition.value);

            if (!conditionMet) return false; // If any condition is not met, the transition fails
        }
        
        // Separately, check for trigger conditions if you decide to add them to the 'conditions' list.
        // A simpler way (used above) is to have one trigger per transition. If you have a trigger, check it.
        // For this example, we assume triggers are also just bool conditions that are true for one frame.
        // For a true "trigger" parameter type, you'd check for it specifically.
        // E.g. if(controller.getTrigger(triggerParameterName)) { return true; }

        return true;
    }

} // namespace Engine