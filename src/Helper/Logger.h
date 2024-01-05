#pragma once

#include <string>
#include <format>
#include <fstream>
#include <mutex>

namespace Helper
{
    class Logger
    {
    public:
        enum class Level: int
        {
            Info = 0,
            Warning = 1,
            Error = 2
        };

    public:
        Logger() = delete;

    public:
        static void InitConsoleLogger();
        static void InitFileLogger(const std::string& loggerPath);
        static void Close();

    public:
        static void SetFilterLevel(Level targetLevel);
        static Level GetCurrentFilterLevel();

        static void LogInfo(const std::string& message);
        static void LogInfo(const char* message);

        template <class... Types>
        static void LogInfo(const std::format_string<Types...> Fmt, Types&&... Args)
        {
            LogInfo(std::format(Fmt, std::forward<Types>(Args)...));
        }

        static void LogWarn(const std::string& message);
        static void LogWarn(const char* message);

        template <class... Types>
        static void LogWarn(const std::format_string<Types...> Fmt, Types&&... Args)
        {
            LogWarn(std::format(Fmt, std::forward<Types>(Args)...));
        }

        static void LogError(const std::string& message);
        static void LogError(const char* message);

        template <class... Types>
        static void LogError(const std::format_string<Types...> Fmt, Types&&... Args)
        {
            LogError(std::format(Fmt, std::forward<Types>(Args)...));
        }

    private:
        inline static bool _enableConsoleLogger = false;
        inline static bool _enableFileLogger = false;
        inline static std::mutex _mutex = {};
        inline static std::fstream* _pFileStream = nullptr;
        inline static Level _filterLevel = Level::Info;
    };
}
