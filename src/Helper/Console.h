#pragma once

#include "PlatformDefine.h"

namespace Helper
{
    class Console
    {
    public:
        enum class Color
        {
            None,
            Green,
            Red,
            Blue,
            White,
            Black,
            Yellow,
            Purple,
            Cyan
        };

    public:
        Console() = delete;

    public:
        static void CreateConsole();
        static void AttachConsole();
        static void DetachConsole();
        static void SetConsoleOutputUtf8();
        static void SetStdOutColor(Color foreground, Color background, bool foregroundIntensity = false, bool backgroundIntensity = false);
        static void ClearScreen();
    };
}