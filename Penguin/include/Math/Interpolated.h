#pragma once
#include <SDL3/SDL_timer.h>
#include "MathUtils.h" 

namespace pgn {

    enum class Transition { None, Linear, SmoothStep, EaseInOutExp, EaseOutBack, EaseInBack, EaseOutElastic, EaseOutBounce, EaseInSine };

    template<typename T>
    class Interpolated
    {
    public:
        explicit Interpolated(T const& initialValue = {}): m_start{ initialValue }, m_end{ m_start }
        {
            m_startTime = getCurrentTime();
        }

        /** Sets a new target value. Starts interpolation from the current visual position. */
        void setValue(T const& newValue) 
        {
            m_start = getValue(); 
            m_end = newValue;
            m_startTime = getCurrentTime();
        }

        /** Sets how long the transition should take in seconds. */
        void setDuration(float seconds) { m_speed = (seconds > 0.0f) ? (1.0f / seconds) : 100000.0f; }

        void setTransitionType(Transition type) { m_transition = type; }

        /** Returns the current interpolated value based on time. */
        [[nodiscard]] T getValue() const 
        {
            float t = getProgress();

            if (t >= 1.0f) return m_end;
            if (t <= 0.0f) return m_start;
            
            return Math::Lerp(m_start, m_end, calculateRatio(t, m_transition));
        }

        /** Returns true if the animation has reached its destination. */
        [[nodiscard]] bool isFinished() const { return getProgress() >= 1.0f; }

        /** Returns a value from 0.0 to 1.0 representing animation progress. */
        [[nodiscard]] float getProgress() const
        {
            float elapsed = static_cast<float>(getCurrentTime() - m_startTime);
            return Math::Clamp(elapsed * m_speed, 0.0f, 1.0f);
        }

        void operator=(T const& newValue) { setValue(newValue); }
        [[nodiscard]] operator T() const { return getValue(); }

    private:
        T m_start{};
        T m_end{};
        double m_startTime{ 0.0 };
        float m_speed{ 1.0f };
        Transition m_transition{ Transition::Linear };

        static double getCurrentTime() { return static_cast<double>(SDL_GetTicksNS()) / 1'000'000'000.0; }

        static float calculateRatio(float t, Transition type) 
        {
            switch (type) 
            {
                case Transition::None:         return 1.0f;
                case Transition::Linear:       return t;
                case Transition::SmoothStep:   return t * t * (3.0f - 2.0f * t);
                case Transition::EaseInSine:   return 1.0f - Math::Cos((t * Math::PI) / 2.0f);
                case Transition::EaseInOutExp: return (t < 0.5f) ? std::pow(2.0f, 20.0f * t - 10.0f) * 0.5f : (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) * 0.5f;
                case Transition::EaseOutBack: 
                {
                    const float c1 = 1.70158f;
                    const float c3 = c1 + 1.0f;
                    return 1.0f + c3 * std::pow(t - 1.0f, 3) + c1 * std::pow(t - 1.0f, 2);
                }
                case Transition::EaseOutElastic: 
                {
                    const float c4 = (2.0f * Math::PI) / 3.0f;
                    return (t == 0.0f) ? 0.0f : (t == 1.0f) ? 1.0f : std::pow(2.0f, -10.0f * t) * Math::Sin((t * 10.0f - 0.75f) * c4) + 1.0f;
                }
                case Transition::EaseOutBounce: 
                {
                    const float n1 = 7.5625f;
                    const float d1 = 2.75f;
                    if      (t < 1.0f / d1) return n1 * t * t;
                    else if (t < 2.0f / d1) return n1 * (t -= 1.5f / d1) * t + 0.75f;
                    else if (t < 2.5f / d1) return n1 * (t -= 2.25f / d1) * t + 0.9375f;
                    else                    return n1 * (t -= 2.625f / d1) * t + 0.984375f;
                }
                default: return t;
            }
        }
    };
}