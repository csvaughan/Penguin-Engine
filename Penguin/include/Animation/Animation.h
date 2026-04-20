#pragma once
#include "pgpch.h"
#include "Math/Rect.h"

namespace pgn {

    enum class LoopMode {
        Single,     // Play once and stop
        Loop,       // 1, 2, 3, 1, 2, 3...
        PingPong    // 1, 2, 3, 2, 1...
    };

    struct AnimationFrame {
        IntRect rect;
        float duration; // Duration for this specific frame
    };

    class Animation : public RefCounted
    {
    public:
        static Ref<Animation> CreateFromSheet(
            const Ref<Texture>& texture, 
            uint32_t frameW, uint32_t frameH, 
            float defaultDuration, 
            LoopMode mode = LoopMode::Loop) 
        {
            auto clip = CreateRef<Animation>();

            clip->m_frameSize = {(float)frameW, (float)frameH};
            clip->m_texture = texture; // Keep reference to source texture
            clip->m_loopMode = mode;

            int framesPerRow = (int)texture->getWidth() / frameW;
            int framesPerCol = (int)texture->getHeight() / frameH;
            
            for (int y = 0; y < framesPerCol; y++) 
            {
                for (int x = 0; x < framesPerRow; x++) 
                {
                    AnimationFrame frame;
                    frame.rect = { (int)(x * frameW), (int)(y * frameH), (int)frameW, (int)frameH };
                    frame.duration = defaultDuration;
                    clip->addFrame(frame);
                }
            }
            return clip;
        }

        void addFrame(const AnimationFrame& frame) 
        {
            m_frames.push_back(frame);
            m_totalDuration += frame.duration;
        }

        // Getters
        const std::vector<AnimationFrame>& getFrames() const { return m_frames; }
        float getTotalDuration() const { return m_totalDuration; }
        LoopMode getLoopMode() const { return m_loopMode; }
        WeakRef<Texture> getTexture() const { return m_texture; }
        const Vector2 getFrameSize() const {return m_frameSize; }

    private:
        std::vector<AnimationFrame> m_frames;
        WeakRef<Texture> m_texture;
        Vector2 m_frameSize;
        float m_totalDuration = 0.0f;
        LoopMode m_loopMode = LoopMode::Loop;
    };
}