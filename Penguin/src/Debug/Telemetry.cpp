#include "Debug/MemoryTelemetry.h"
#include <cstdlib>
#include <new>

#ifdef PGN_DEBUG

namespace pgn {
    static MemoryStats s_Stats;
    MemoryStats& GetMemoryStats() { return s_Stats; }
}

// Global Overrides
// Note: These sit outside the namespace
void* operator new(size_t size) {
    auto& stats = pgn::GetMemoryStats();
    stats.TotalAllocated += size;
    stats.AllocationCount++;
    
    // Update Peak if current is higher
    size_t current = stats.CurrentUsage();
    if (current > stats.PeakUsage) stats.PeakUsage = current;

    return malloc(size);
}

void operator delete(void* memory, size_t size) noexcept {
    pgn::GetMemoryStats().TotalFreed += size;
    free(memory);
}

void operator delete(void* memory) noexcept {
    // Note: Standard delete doesn't always provide size
    free(memory);
}

#endif