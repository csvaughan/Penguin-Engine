#pragma once
#include "Core/ImGuiLayer.h"

namespace pgn
{
    class DebugLayer final : public ImguiLayer
    {
        public:
            DebugLayer(const std::string& name) : ImguiLayer(name) {}
            ~DebugLayer(){}

            void OnEnter() override {}

            void OnImGuiUpdate(Timestep ts) override
            {   
                double time = App().GetTime();
                double secondsTotal = 0;
                double fractionalPart = std::modf(time, &secondsTotal);
                upTime.minutes = static_cast<int>(secondsTotal) / 60;
                upTime.seconds = static_cast<int>(secondsTotal) % 60;
                upTime.millis  = static_cast<int>(fractionalPart * 100);
            }

            void OnImGuiRender() override
            {
                drawConsole();
                drawEngineStats();
                drawMeneoryManager();
            }

        private:

            void drawEngineStats();
            void drawConsole();
            void drawMeneoryManager();

        struct upTime
        {
            int minutes;
            int seconds;
            int millis;
        } upTime;
    };
} // namespace pgn