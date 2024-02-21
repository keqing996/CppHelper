#pragma once

#include "../PlatformDefine.h"

#if PLATFORM_ANDROID

#include <string>

namespace Infra
{
    class Android
    {
    public:
        enum LogLevel
        {
            Default,
            Verbose,
            Debug,
            Info,
            Warn,
            Error,
            Fatal,
            Silent
        };

        using LogFunc = void(*)(LogLevel, const char*, const char*);

    public:
        Android() = delete;

    public:
        static LogFunc GetLogFunction();
    };
}

#endif