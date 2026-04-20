#pragma once
#include <spdlog/sinks/base_sink.h>
#include "pgpch.h"
#include <imgui.h> 

namespace pgn {

    struct LogMessage {
        std::string Payload;
        spdlog::level::level_enum Level;
    };

    // Inherit from base_sink<mutex> to get free thread safety for writing
    class ImGuiSink : public spdlog::sinks::base_sink<std::mutex> {
    public:
        // Singleton access: Ensures Log::Init and DebugLayer talk to the SAME object
        static std::shared_ptr<ImGuiSink> GetInstance() {
            static auto instance = std::make_shared<ImGuiSink>();
            return instance;
        }

        // Thread-safe buffer clear
        void Clear() {
            std::lock_guard<std::mutex> lock(mutex_);
            m_Messages.clear();
        }

        // Allow the DebugLayer to lock the mutex while reading
        std::mutex& GetMutex() { return mutex_; }
        
        // Access the raw data
        const std::vector<LogMessage>& GetMessages() const { return m_Messages; }

    protected:
        // This function is called automatically by spdlog when you log something
        void sink_it_(const spdlog::details::log_msg& msg) override {
            spdlog::memory_buf_t formatted;
            base_sink<std::mutex>::formatter_->format(msg, formatted);
            
            // Note: mutex_ is already locked by spdlog during this call
            m_Messages.push_back({ fmt::to_string(formatted), msg.level });

            // Optional: Keep buffer size manageable (e.g., max 2000 lines)
            if (m_Messages.size() > 2000)
                m_Messages.erase(m_Messages.begin());
        }

        void flush_() override {}

    private:
        std::vector<LogMessage> m_Messages;
    };
}