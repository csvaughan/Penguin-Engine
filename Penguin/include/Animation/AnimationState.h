#pragma once
#include "EnginePCH.h"
#include "Animation.h"

namespace Engine
{
    class AnimationControllerComponent;

    // The type of comparison to perform for a condition.
    enum class EComparisonMode
    {
        Equals,
        NotEquals,
        GreaterThan,
        LessThan,
        GreaterThanOrEqual,
        LessThanOrEqual
    };

    // A single condition that must be met for a transition to occur.
    struct Condition
    {
        std::string parameterName;
        EComparisonMode mode;
        std::variant<bool, int, float> value;
    };

    // A transition between two animation states.
    struct Transition
    {
        std::string targetStateName;
        std::vector<Condition> conditions;

        // Checks if all conditions for this transition are met.
        bool AreConditionsMet(const AnimationControllerComponent& controller) const;
    };

    struct AnimationState
    {
        Animation animation;
        std::vector<Transition> transitions;
    };

} // namespace Engine