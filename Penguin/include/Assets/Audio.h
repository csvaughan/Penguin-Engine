#pragma once
#include <miniaudio/miniaudio.h>
#include "Log/Log.h"
#include "Audio/AudioType.h"
#include "Memory/Ref.h"

namespace pgn {

    class Audio : public RefCounted
    {
    public:
        Audio(ma_engine* engine, ::ma_sound_group* group, const std::string& path, AudioType type = AudioType::Effect) 
        {
            ma_uint32 flags = 0;
            
            if (type == AudioType::Effect) { flags |= MA_SOUND_FLAG_DECODE; }
            flags |= MA_SOUND_FLAG_ASYNC; 

            ma_result result = ma_sound_init_from_file(engine, path.c_str(), flags, group, NULL, &m_internalAudio);
            m_initialized = (result != MA_SUCCESS) ? false : true;
        }

        ~Audio() { if (m_initialized) ma_sound_uninit(&m_internalAudio); }

        Audio(const Audio&) = delete;
        Audio& operator=(const Audio&) = delete;

        Audio(Audio&&) = delete;

        ma_sound* GetInternal() { return &m_internalAudio; }

    private:
        ma_sound m_internalAudio;
        bool m_initialized = false;
        
        friend class AudioSystem;
    };
}