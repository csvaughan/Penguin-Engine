#include "Audio/Audio.h"
#include <miniaudio/miniaudio.h>

namespace pgn {

    Audio::Audio(ma_engine* engine, ::ma_sound_group* group, const std::string& path, AudioType type)
    {
        if (!engine) return;
        m_internalAudio = new ma_sound(); 

        ma_uint32 flags = 0;
        if (type == AudioType::Effect) { flags |= MA_SOUND_FLAG_DECODE; }
        flags |= MA_SOUND_FLAG_ASYNC;

        // 2. Pass pointer directly (not &m_internalAudio)
        ma_result result = ma_sound_init_from_file(engine, path.c_str(), flags, group, NULL, m_internalAudio);
        
        if (result != MA_SUCCESS) 
        {
            delete m_internalAudio; // Cleanup if init fails
            m_internalAudio = nullptr;
            m_initialized = false;
            return;
        }

        m_initialized = true;
    }

    Audio::~Audio()
    {
        if (m_initialized && m_internalAudio) 
        {
            ma_sound_uninit(m_internalAudio);
            delete m_internalAudio; 
            m_internalAudio = nullptr;
            m_initialized = false;
        }
    }
}