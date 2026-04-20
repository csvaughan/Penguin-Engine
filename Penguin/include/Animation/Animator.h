#pragma once
#include "Animation.h"
#include "Renderer/Sprite.h"

namespace pgn {

    class Animator 
    {
    public:
        Animator() = default;

        void play(Ref<Animation> clip) 
        {
            if (m_currentClip == clip) return; // Don't reset if already playing

            m_currentClip = clip;
            m_elapsedTime = 0.0f;
            m_currentFrameIndex = 0;
            m_isFinished = false;
        }

        void update(float dt, Sprite& targetSprite) 
        {
            if (!m_currentClip || m_currentClip->getFrames().empty()) return;
            if (m_isFinished) return;

            m_elapsedTime += dt;

            const auto& frames = m_currentClip->getFrames();
            float duration = m_currentClip->getTotalDuration();
            LoopMode mode = m_currentClip->getLoopMode();

            // Update the Texture (only needed if the clip changed)
            if (targetSprite.getTexture() != m_currentClip->getTexture()) 
                targetSprite.setTexture(m_currentClip->getTexture().lock(), false);

            switch (mode)
            {
                case LoopMode::Single:
                {
                    if (m_elapsedTime >= duration) 
                    {
                        m_elapsedTime = duration;
                        m_currentFrameIndex = frames.size() - 1;
                        m_isFinished = true;
                    } 
                    else {
                        m_currentFrameIndex = calculateFrameIndex(m_elapsedTime);
                    }
                    break;
                }
                case LoopMode::Loop:
                {
                    // Wrap time modulo total duration
                    float loopedTime = fmod(m_elapsedTime, duration);
                    m_currentFrameIndex = calculateFrameIndex(loopedTime);
                    break;
                }
                case LoopMode::PingPong:
                {
                    // PingPong logic: time goes 0 -> duration -> 0
                    float pingPongTime = fmod(m_elapsedTime, duration * 2.0f);
                    if (pingPongTime > duration) {
                        // Backward phase
                        float backwardTime = (duration * 2.0f) - pingPongTime;
                        m_currentFrameIndex = calculateFrameIndex(backwardTime);
                    } else {
                        // Forward phase
                        m_currentFrameIndex = calculateFrameIndex(pingPongTime);
                    }
                    break;
                }
            }
            
            targetSprite.setTextureRect(frames[m_currentFrameIndex].rect);
        }

        bool isFinished() const { return m_isFinished; }

        void resetAnimation() 
        {
            m_currentClip = nullptr;
            m_elapsedTime = 0.0f;
            m_currentFrameIndex = 0;
            m_isFinished = false;
        }
        
        // Helper to set specific frame manually
        void setFrameIndex(size_t index) 
        {
            if (m_currentClip && index < m_currentClip->getFrames().size()) 
            {
                m_currentFrameIndex = index;
                m_elapsedTime = 0.0f; // Reset time to start of this frame to avoid jumping
                for(size_t i=0; i<index; ++i) m_elapsedTime += m_currentClip->getFrames()[i].duration;
            }
        }

    private:
        size_t calculateFrameIndex(float timeInCycle) 
        {
            float t = 0.0f;
            const auto& frames = m_currentClip->getFrames();
            for (size_t i = 0; i < frames.size(); ++i) 
            {
                t += frames[i].duration;
                if (t >= timeInCycle) return i;
            }
            return frames.size() - 1;
        }

        Ref<Animation> m_currentClip;
        float m_elapsedTime = 0.0f;
        size_t m_currentFrameIndex = 0;
        bool m_isFinished = false;
    };
}