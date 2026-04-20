#pragma once
#include "Math/Vector.h"
#include "Math/Rect.h"
#include "Math/Transform.h"
#include <spdlog/logger.h>

// --- Custom Type Formatters ---

template<>
struct fmt::formatter<pgn::Vector2> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const pgn::Vector2& v, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "({}, {})", v.x, v.y);
    }
};

template<>
struct fmt::formatter<pgn::Vector3> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const pgn::Vector3& v, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
    }
};

template<>
struct fmt::formatter<pgn::Vector4> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const pgn::Vector4& v, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "({}, {}, {}, {})", v.x, v.y, v.z, v.w);
    }
};

template<typename T>
struct fmt::formatter<pgn::Rect<T>> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const pgn::Rect<T>& r, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "[Pos: ({}, {}), Size: {}x{}]", r.x, r.y, r.w, r.h);
    }
};

template<>
struct fmt::formatter<pgn::Transform2D> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const pgn::Transform2D& t, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "Transform(Pos: {}, Rot: {:.2f}, Scale: {})", 
            t.position, t.rotation, t.scale);
    }
};

namespace spdlog { class logger;}

namespace pgn {

	enum class LogLevel { Trace = 0, Info, Warn, Error, Critical, None };

    class Log 
	{
    public:
        static void Init(LogLevel level = LogLevel::Trace);
        static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

		static void SetLevel(LogLevel);

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}

// Debug-only Macro Logic
#ifdef PGN_DEBUG

	#ifdef PGN_ENGINE_EXPORTS
        #define PGN_CORE_TRACE(...)    ::pgn::Log::GetCoreLogger()->trace(__VA_ARGS__)
        #define PGN_CORE_INFO(...)     ::pgn::Log::GetCoreLogger()->info(__VA_ARGS__)
        #define PGN_CORE_WARN(...)     ::pgn::Log::GetCoreLogger()->warn(__VA_ARGS__)
        #define PGN_CORE_ERROR(...)    ::pgn::Log::GetCoreLogger()->error(__VA_ARGS__)
        #define PGN_CORE_CRITICAL(...) ::pgn::Log::GetCoreLogger()->critical(__VA_ARGS__)
        
    #endif

    // --- CLIENT LOGGING (Always available in Debug) ---
    #define PGN_TRACE(...)         ::pgn::Log::GetClientLogger()->trace(__VA_ARGS__)
    #define PGN_INFO(...)          ::pgn::Log::GetClientLogger()->info(__VA_ARGS__)
    #define PGN_WARN(...)          ::pgn::Log::GetClientLogger()->warn(__VA_ARGS__)
    #define PGN_ERROR(...)         ::pgn::Log::GetClientLogger()->error(__VA_ARGS__)
    #define PGN_CRITICAL(...)      ::pgn::Log::GetClientLogger()->critical(__VA_ARGS__)

	//Assert
	#if defined(MSVC_VERSION)
        #define PGN_BREAK() __debugbreak()
    #else
        #include <signal.h>
        #define PGN_BREAK() raise(SIGTRAP)
    #endif

    #ifdef PGN_ENGINE_EXPORTS
        #define PGN_ASSERT(condition, ...) { if(!(condition)) { PGN_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); PGN_BREAK(); } }
    #else
        #define PGN_ASSERT(condition, ...) { if(!(condition)) { PGN_ERROR("Assertion Failed: {0}", __VA_ARGS__); PGN_BREAK(); } }
    #endif
#else
    #define PGN_CORE_TRACE(...)
    #define PGN_CORE_INFO(...)
    #define PGN_CORE_WARN(...)
    #define PGN_CORE_ERROR(...)
    #define PGN_CORE_CRITICAL(...)

    #define PGN_TRACE(...)
    #define PGN_INFO(...)
    #define PGN_WARN(...)
    #define PGN_ERROR(...)
    #define PGN_CRITICAL(...)
	#define PGN_ASSERT(condition, ...)
#endif