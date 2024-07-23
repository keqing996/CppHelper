#pragma once

#include <vector>
#include <string>

#ifdef __clang__
#   if __clang_major__ >= 17
#       define HAVE_STD_FORMAT 1
#   else
#       define HAVE_STD_FORMAT 0
#   endif
#else
#   define HAVE_STD_FORMAT 1
#endif

#if HAVE_STD_FORMAT
#include <format>
#endif

namespace Infra
{
    class Logger
    {
    public:
        using LogCallBack = void(*)(const char*);

        enum class Level: int
        {
            Info = 0,
            Warning = 1,
            Error = 2
        };

    public:
        Logger() = delete;

    public:
        static void SetFilterLevel(Level targetLevel);

        static Level GetCurrentFilterLevel();

        template <Level level>
        static void AddLogCall(LogCallBack pFunc)
        {
            if constexpr (level == Level::Info)
                _logInfoCallVec.push_back(pFunc);
            else if constexpr (level == Level::Warning)
                _logWarnCallVec.push_back(pFunc);
            else if constexpr (level == Level::Error)
                _logErrorCallVec.push_back(pFunc);
        }

        // Log info
        static void LogInfo(const std::string& message);
        static void LogInfo(const char* message);

        // Log warn
        static void LogWarn(const std::string& message);
        static void LogWarn(const char* message);

        // Log error
        static void LogError(const std::string& message);
        static void LogError(const char* message);

#if HAVE_STD_FORMAT
        template <class... Types>
        static void LogInfo(std::format_string<Types...> Fmt, Types&&... Args)
        {
            LogInfo(std::format(Fmt, std::forward<Types>(Args)...));
        }

        template <class... Types>
        static void LogWarn(std::format_string<Types...> Fmt, Types&&... Args)
        {
            LogWarn(std::format(Fmt, std::forward<Types>(Args)...));
        }

        template <class... Types>
        static void LogError(std::format_string<Types...> Fmt, Types&&... Args)
        {
            LogError(std::format(Fmt, std::forward<Types>(Args)...));
        }
#endif

    private:
        inline static Level _filterLevel = Level::Info;

        inline static std::vector<LogCallBack> _logInfoCallVec {};
        inline static std::vector<LogCallBack> _logWarnCallVec {};
        inline static std::vector<LogCallBack> _logErrorCallVec {};
    };
}
