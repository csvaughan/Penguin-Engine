#pragma once
#include "Audio/AudioType.h"
#include "Asset/IAsset.h"

struct ma_engine;
struct ma_sound; 
typedef struct ma_sound ma_sound_group;

namespace pgn {

    class Audio : public IAsset
    {
    public:
        Audio(ma_engine* engine, ::ma_sound_group* group, const std::string& path, AudioType type);
        ~Audio();

        Audio(const Audio&) = delete;
        Audio& operator=(const Audio&) = delete;

        Audio(Audio&&) = delete;

    private:
        ma_sound* m_internalAudio;
        bool m_initialized = false;
        
        friend class AudioSystem;
    };

    class SoundEffect : public Audio {
    public:
        SoundEffect(ma_engine* engine, ma_sound_group* group, const std::string& path)
            : Audio(engine, group, path, AudioType::Effect) {}
    };

    class Music : public Audio {
    public:
        Music(ma_engine* engine, ma_sound_group* group, const std::string& path)
            : Audio(engine, group, path, AudioType::Music) {}
    };
}