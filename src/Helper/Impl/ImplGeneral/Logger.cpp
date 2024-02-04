
#include "../../Logger.h"
#include "../../WinApi/WinApiConsole.h"

#include <iostream>

namespace Helper
{
    void Logger::SetFilterLevel(Level targetLevel)
    {
        _filterLevel = targetLevel;
    }

    Logger::Level Logger::GetCurrentFilterLevel()
    {
        return _filterLevel;
    }

    void Logger::InitConsoleLogger()
    {
#ifdef ENABLE_CONSOLE_LOG
        Helper::Win::Console::CreateConsole();
        Helper::Win::Console::SetConsoleOutputUtf8();
        _enableConsoleLogger = true;
#endif
    }

    void Logger::InitFileLogger(const std::string& loggerPath)
    {
        if (_pFileStream != nullptr)
            return;

        _pFileStream = new std::fstream(loggerPath, std::ios::out | std::ios::trunc);
        if (!_pFileStream->is_open())
        {
            delete _pFileStream;
            return;
        }

        _enableFileLogger = true;
    }

    void Logger::Close()
    {
        if (_pFileStream != nullptr)
        {
            if (_pFileStream->is_open())
                _pFileStream->close();

            delete _pFileStream;
        }

#ifdef ENABLE_CONSOLE_LOG
        Helper::Win::Console::DetachConsole();
#endif
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

        if (_enableConsoleLogger)
        {
            Win::Console::SetColor(Win::Console::Color::White, Win::Console::Color::None);
            std::cout << "[I] " << message << std::endl;
        }

        if (_enableFileLogger && _pFileStream != nullptr)
        {
            (*_pFileStream) << "[I] " << message << std::endl;
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

        if (_enableConsoleLogger)
        {
            Win::Console::SetColor(Win::Console::Color::Yellow, Win::Console::Color::None);
            std::cout << "[W] " << message << std::endl;
            Win::Console::SetColor(Win::Console::Color::White, Win::Console::Color::None);
        }

        if (_enableFileLogger && _pFileStream != nullptr)
        {
            (*_pFileStream) << "[W] " << message << std::endl;
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

        if (_enableConsoleLogger)
        {
            Win::Console::SetColor(Win::Console::Color::Red, Win::Console::Color::None);
            std::cout << "[E] " << message << std::endl;
            Win::Console::SetColor(Win::Console::Color::White, Win::Console::Color::None);
        }

        if (_enableFileLogger && _pFileStream != nullptr)
        {
            (*_pFileStream) << "[E] " << message << std::endl;
        }
    }
}