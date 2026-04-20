#include "Audio/AudioSystem.h"
#include "Assets/Audio.h" 
#include "Math/Vector.h"
#include <glm/common.hpp>

namespace pgn
{
    struct AudioData 
    {
        ma_engine Engine;
        ma_sound_group MusicGroup;
        ma_sound_group SFXGroup;
        bool Initialized = false;

        float MasterVolume = 1;
        float SFXVolume = 1;
        float MusicVolume = 1;
    };

    static AudioData* s_Data = nullptr;

    void AudioSystem::Init() 
    {
        if (s_Data) return;
        s_Data = new AudioData();
        
        // Initialize Engine
        ma_result result = ma_engine_init(NULL, &s_Data->Engine);
        if (result != MA_SUCCESS) 
        {
            PGN_CORE_ERROR("AudioSystem: Failed to initialize miniaudio engine!");
            return;
        }

        // Setup Defaults
        ma_engine_listener_set_position(&s_Data->Engine, 0, 0, 0, 0); // Listener at 0,0
        ma_engine_listener_set_world_up(&s_Data->Engine, 0, 0, 0, 1); // Z-up is standard for 2D audio usually
        ma_engine_set_volume(&s_Data->Engine, 1.0f);

        // Create the Music and SFX Groups
        ma_sound_group_init(&s_Data->Engine, 0, nullptr, &s_Data->MusicGroup);
        ma_sound_group_init(&s_Data->Engine, 0, nullptr, &s_Data->SFXGroup);

        s_Data->Initialized = true;
        PGN_CORE_INFO("AudioSystem Initialized Successfully.");
    }

    void AudioSystem::Shutdown() 
    {
        if (!s_Data) return;

        ma_engine_uninit(&s_Data->Engine);
        delete s_Data;
        s_Data = nullptr;
        
        PGN_CORE_INFO("AudioSystem Shutdown Successfully...");
    }

    void AudioSystem::OnUpdate(Vector2& listenerPos)
    {
        if (!s_Data || !s_Data->Initialized) return;
        
        // Update listener position so 3D sounds pan correctly
        // (Listener Index 0, X, Y, Z)
        ma_engine_listener_set_position(&s_Data->Engine, 0, listenerPos.x, listenerPos.y, 0.0f);
    }

    void AudioSystem::Play(Ref<Audio> sound, float volume, float pitch, bool loop) 
    {
        if (!sound || !s_Data) return;

        ma_sound* internal = sound->GetInternal();
        
        // Ensure this sound is treated as "Global" (2D/UI)
        ma_sound_set_spatialization_enabled(internal, MA_FALSE);

        ma_sound_set_volume(internal, volume);
        ma_sound_set_pitch(internal, pitch);
        ma_sound_set_looping(internal, loop ? MA_TRUE : MA_FALSE);
        
        // Rewind in case we are re-using a sound
        ma_sound_seek_to_pcm_frame(internal, 0);
        
        ma_sound_start(internal);
    }

    void AudioSystem::PlayAtLocation(Ref<Audio> sound, float x, float y) 
    {
        if (!sound || !s_Data) return;
        ma_sound* internal = sound->GetInternal();
        
        // Enable 3D Spatialization
        ma_sound_set_spatialization_enabled(internal, MA_TRUE);
        
        // Miniaudio uses a default attenuation model, but you can tweak it in Init if needed
        ma_sound_set_position(internal, x, y, 0.0f);
        
        // Rewind and Play
        ma_sound_seek_to_pcm_frame(internal, 0);
        ma_sound_start(internal);
    }

    void AudioSystem::Stop(Ref<Audio> sound) { if (sound) ma_sound_stop(sound->GetInternal()); }

    void AudioSystem::SetMasterVolume(float volume) 
    {
        if (!s_Data || !s_Data->Initialized) return;
        s_Data->MasterVolume = glm::clamp(volume, 0.0f, 1.0f);
        ma_engine_set_volume(&s_Data->Engine, s_Data->MasterVolume);
    }

    void AudioSystem::SetMusicVolume(float volume) 
    {
        if (!s_Data || !s_Data->Initialized) return;
        s_Data->MusicVolume = glm::clamp(volume, 0.0f, 1.0f);
        if (s_Data) ma_sound_group_set_volume(&s_Data->MusicGroup, volume);
    }

    void AudioSystem::SetSFXVolume(float volume) 
    {
        if (!s_Data || !s_Data->Initialized) return;
        s_Data->SFXVolume = glm::clamp(volume, 0.0f, 1.0f);
        ma_sound_group_set_volume(&s_Data->SFXGroup, volume);
    }

    float pgn::AudioSystem::GetMusicVolume() { return s_Data ? s_Data->MusicVolume : 1.0f; }
    float AudioSystem::GetMasterVolume() { return s_Data ? s_Data->MasterVolume : 1.0f; }
    float pgn::AudioSystem::GetSFXVolume(){ return s_Data ? s_Data->SFXVolume : 1.0f; }

    ma_sound_group* AudioSystem::GetSFXGroup() { return s_Data ? &s_Data->SFXGroup : nullptr; }
    ma_sound_group* AudioSystem::GetMusicGroup() { return s_Data ? &s_Data->MusicGroup : nullptr; }
    ma_engine* AudioSystem::GetEngine() { return (s_Data) ? &s_Data->Engine : nullptr; }
} // namespace pgn