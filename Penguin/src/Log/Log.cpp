#include "Log/Log.h"
#include "Log/ImGuiSink.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace pgn {

    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init(LogLevel level)
    {
        // Guard against double-initialization
        if (s_CoreLogger) 
		{
			std::println(std::cerr, "Warning: Logging system already initialized."); 
			return; 
		}

        try {
            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("Penguin.log", true);
            auto imguiSink = ImGuiSink::GetInstance();

            consoleSink->set_pattern("%^[%T] %n: %v%$");
            fileSink->set_pattern("[%T] [%l] %n: %v");
            imguiSink->set_pattern("%^[%T] %n: %v%$"); 

            std::vector<spdlog::sink_ptr> sinks { consoleSink, fileSink, imguiSink };

            s_CoreLogger = std::make_shared<spdlog::logger>("PENGUIN", sinks.begin(), sinks.end());
            spdlog::register_logger(s_CoreLogger);

            s_ClientLogger = std::make_shared<spdlog::logger>("APP", sinks.begin(), sinks.end());
            spdlog::register_logger(s_ClientLogger);

            SetLevel(level);
            
            // Test log to verify it works immediately
            PGN_CORE_INFO("Logging System Initialized");

        } catch (const spdlog::spdlog_ex& ex) {
            std::println(std::cerr, "Log init failed: {}", ex.what());
        }
    }

    void Log::SetLevel(LogLevel level)
	{
		spdlog::level::level_enum spdLevel;

		switch (level) 
		{
			case LogLevel::Trace:    spdLevel = spdlog::level::trace; break;
			case LogLevel::Info:     spdLevel = spdlog::level::info; break;
			case LogLevel::Warn:     spdLevel = spdlog::level::warn; break;
			case LogLevel::Error:    spdLevel = spdlog::level::err; break;
			case LogLevel::Critical: spdLevel = spdlog::level::critical; break;
			case LogLevel::None:     spdLevel = spdlog::level::off; break;
			default:                 spdLevel = spdlog::level::info;
		}

		s_CoreLogger->set_level(spdLevel);
		s_ClientLogger->set_level(spdLevel);

		s_CoreLogger->flush_on(spdLevel);
    	s_ClientLogger->flush_on(spdLevel);
		PGN_CORE_INFO("Logging level set to {}.", static_cast<int>(level));
	}
}