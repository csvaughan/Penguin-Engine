#pragma once
#include <atomic>
#include <cstdint>

namespace pgn {
    
    struct MemoryStats {
        std::atomic<size_t> TotalAllocated = 0;
        std::atomic<size_t> TotalFreed = 0;
        std::atomic<uint32_t> AllocationCount = 0;
        std::atomic<size_t> PeakUsage = 0; // Added Peak

        size_t CurrentUsage() const { return TotalAllocated - TotalFreed; }
    };

    // Global access to stats
    MemoryStats& GetMemoryStats();
}