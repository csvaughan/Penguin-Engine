#pragma once

namespace pgn
{
    enum class AudioType 
    {
        Effect, // Loads into RAM (Fast playback, high memory)
        Music   // Streams from Disk (Low memory, slight CPU cost)
    };
} // namespace pgn
