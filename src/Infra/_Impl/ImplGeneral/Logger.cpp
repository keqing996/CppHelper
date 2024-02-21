
#include "../../Logger.h"

#include <iostream>

namespace Infra
{
    void Logger::SetFilterLevel(Level targetLevel)
    {
        _filterLevel = targetLevel;
    }

    Logger::Level Logger::GetCurrentFilterLevel()
    {
        return _filterLevel;
    }

    void Logger::LogInfo(const std::string& message)
    {
        if (static_cast<int>(_filterLevel) > static_cast<int>(Logger::Level::Info))
            return;

        LogInfo(message.c_str());
    }

    void Logger::LogInfo(const char* message)
    {
        if (static_cast<int>(_filterLevel) > static_cast<int>(Logger::Level::Info))
            return;

        std::lock_guard<std::mutex> guard(_mutex);
        {
            for (const auto p: _logInfoCallVec)
            {
                if (p != nullptr)
                    p(message);
            }
        }
    }

    void Logger::LogWarn(const std::string& message)
    {
        if (static_cast<int>(_filterLevel) > static_cast<int>(Logger::Level::Warning))
            return;

        LogWarn(message.c_str());
    }

    void Logger::LogWarn(const char* message)
    {
        if (static_cast<int>(_filterLevel) > static_cast<int>(Logger::Level::Warning))
            return;

        std::lock_guard<std::mutex> guard(_mutex);
        {
            for (const auto p: _logWarnCallVec)
            {
                if (p != nullptr)
                    p(message);
            }
        }
    }

    void Logger::LogError(const std::string& message)
    {
        if (static_cast<int>(_filterLevel) > static_cast<int>(Logger::Level::Error))
            return;

        LogError(message.c_str());
    }

    void Logger::LogError(const char* message)
    {
        if (static_cast<int>(_filterLevel) > static_cast<int>(Logger::Level::Error))
            return;

        std::lock_guard<std::mutex> guard(_mutex);
        {
            for (const auto p: _logErrorCallVec)
            {
                if (p != nullptr)
                    p(message);
            }
        }
    }
}