#pragma once
#include "Audio.h"
#include "pgpch.h"

namespace pgn {

    class Audio; 
    class Vector2;

    class AudioSystem 
    {
    public:
        // Lifecycle
        static void Init();
        static void Shutdown();

        // Listener
        static void OnUpdate(Vector2& listenerPosition);

        static void SetMasterVolume(float volume);
        static void SetMusicVolume(float volume);
        static void SetSFXVolume(float volume);
        static float GetMasterVolume();
        static float GetMusicVolume();
        static float GetSFXVolume();

        // Playback
        static void Play(Ref<Audio> sound, float volume = 1.0f, float pitch = 1.0f, bool loop = false);
        static void PlayAtLocation(Ref<Audio> sound, float x, float y);
        static void Stop(Ref<Audio> sound); 

    private:
        static ma_engine* GetEngine();
        static ma_sound_group* GetSFXGroup();
        static ma_sound_group* GetMusicGroup();

        friend class SoundEffectImporter;
        friend class MusicImporter;
    };
}