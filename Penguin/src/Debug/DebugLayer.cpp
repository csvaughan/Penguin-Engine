#include "Debug/DebugLayer.h"
#include "Debug/MemoryTelemetry.h"
#include "Log/ImGuiSink.h"
#include "Renderer/Renderer.h"

namespace pgn
{
    void pgn::DebugLayer::drawEngineStats()
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | 
                                ImGuiWindowFlags_AlwaysAutoResize | 
                                ImGuiWindowFlags_NoSavedSettings | 
                                ImGuiWindowFlags_NoFocusOnAppearing | 
                                ImGuiWindowFlags_NoNav;

        ImGui::Begin("Performance Overlay", nullptr, window_flags);
        ImGui::Text("Uptime: %02d:%02d.%02d", upTime.minutes,upTime.seconds, upTime.millis);
        ImGui::Text("FPS: %.2f", App().GetFPS());
        ImGui::Text("Frame Time: %.3f ms", 1000.0f / App().GetFPS());
        
        ImGui::Separator();
        
        // --- Asset Manager Stats ---
        if (ImGui::CollapsingHeader("Assets", ImGuiTreeNodeFlags_DefaultOpen))
        {   
            auto& am = Application::Get().GetAssetManager();
            ImGui::BulletText("Textures: %zu", am.GetTextureCount());
            ImGui::BulletText("Fonts:    %zu", am.GetFontCount());
            ImGui::BulletText("Audio:    %zu", am.GetAudioCount());
        }

        // --- Renderer Stats ---
        if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
        {   
            ImGui::BulletText("Draw calls: %zu", Renderer::GetDrawCallCount());
        }

        //Window
        if (ImGui::CollapsingHeader("Window", ImGuiTreeNodeFlags_DefaultOpen))
        {   
            ImGui::BulletText("Window Dimensions: %.2f x %.2f", App().GetWindow().GetWindowSize().x, App().GetWindow().GetWindowSize().y);
        }

        ImGui::End();
    }

    void pgn::DebugLayer::drawMeneoryManager()
    {
        ImGui::SetNextWindowSize(ImVec2(450, 450), ImGuiCond_FirstUseEver);
        ImGui::Begin("Memory Monitor");

        auto& stats = GetMemoryStats();
        
        // Convert everything to MB for readability
        float currentMB = (float)stats.CurrentUsage() / (1024.0f * 1024.0f);
        float peakMB    = (float)stats.PeakUsage / (1024.0f * 1024.0f);
        float totalMB   = (float)stats.TotalAllocated / (1024.0f * 1024.0f);
        float freedMB   = (float)stats.TotalFreed / (1024.0f * 1024.0f);

        // --- Part 1: Rolling History Logic ---
        static std::vector<float> history(100, 0.0f);
        std::rotate(history.begin(), history.begin() + 1, history.end());
        history.back() = currentMB;

        // --- Part 2: Detailed Stats Table ---
        if (ImGui::BeginTable("MemoryDetails", 2, ImGuiTableFlags_BordersInnerV)) {
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Live Usage:");
            ImGui::SameLine(); ImGui::Text("%.2f MB", currentMB);
            
            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "Peak Usage:");
            ImGui::SameLine(); ImGui::Text("%.2f MB", peakMB);
            
            ImGui::Text("Active Allocs:");
            ImGui::SameLine(); ImGui::Text("%u", stats.AllocationCount.load());

            ImGui::TableNextColumn();
            ImGui::Text("Total Allocated:");
            ImGui::SameLine(); ImGui::Text("%.2f MB", totalMB);
            
            ImGui::Text("Total Freed:");
            ImGui::SameLine(); ImGui::Text("%.2f MB", freedMB);
            
            // Calculate the "Leaked" or "Still In Use" delta
            float delta = totalMB - freedMB;
            ImGui::Text("Delta:");
            ImGui::SameLine(); ImGui::Text("%.2f MB", delta);

            ImGui::EndTable();
        }

        ImGui::Separator();

        // --- Part 3: The Graph ---
        // Auto-scale with a minimum floor of 10MB
        float graphMax = std::max(10.0f, peakMB * 1.1f);
        
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.3f, 0.7f, 1.0f, 1.0f));
        ImGui::PlotLines("##UsageGraph", 
            history.data(), 
            (int)history.size(), 
            0, 
            std::format("{:.2f} MB", currentMB).c_str(), // Current MB as overlay text
            0.0f, 
            graphMax, 
            ImVec2(0, 120)); 
        ImGui::PopStyleColor();

        // --- Part 4: Controls ---
        if (ImGui::Button("Reset Peak")) {
            stats.PeakUsage = stats.CurrentUsage();
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Log Snapshot")) {
            PGN_CORE_INFO("Memory Snapshot: Current: {}MB, Peak: {}MB, Allocs: {}", 
                currentMB, peakMB, stats.AllocationCount.load());
        }

        ImGui::End();
    }

    void pgn::DebugLayer::drawConsole()
    {
        ImGui::Begin("Console");

        // 1. Control Buttons
        if (ImGui::Button("Clear")) { ImGuiSink::GetInstance()->Clear(); }
        ImGui::SameLine();
        if (ImGui::Button("Test Log")) { PGN_INFO("Manual Test Log!"); }

        ImGui::Separator();

        // 2. Create the Scrolling Region
        // Important: passing ImVec2(0,0) makes it fill the remaining window space
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        // 3. Get Sink and LOCK MUTEX
        auto sink = ImGuiSink::GetInstance();
        std::lock_guard<std::mutex> lock(sink->GetMutex());

        // 4. Render Messages
        for (const auto& msg : sink->GetMessages())
        {
            ImVec4 color = {0,1,0,1}; // Default Green

            switch (msg.Level) {
                case spdlog::level::warn:     color = {1.0f, 0.8f, 0.0f, 1.0f}; break; // Yellow
                case spdlog::level::err:      color = {1.0f, 0.2f, 0.2f, 1.0f}; break; // Red
                case spdlog::level::critical: color = {1.0f, 0.0f, 0.0f, 1.0f}; break; // Red
                case spdlog::level::trace:    color = {0.6f, 0.6f, 0.6f, 1.0f}; break; // Gray
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(msg.Payload.c_str());
            ImGui::PopStyleColor();
        }

        // 5. Auto Scroll
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }
} // namespace pgn