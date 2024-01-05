#pragma once

namespace Helper::Win::Console
{
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

    void CreateConsole();

    void AttachConsole();

    void DetachConsole();

    void SetConsoleOutputUtf8();

    void* GetStdOutputHandle();

    void* GetStdInputHandle();

    void* GetStdErrorHandle();

    void SetWindowResizeEnable(bool enable);

    void SetWindowMaxEnable(bool enable);

    void SetWindowMinEnable(bool enable);

    void SetColor(Color foreground, Color background, bool foregroundIntensity = false, bool backgroundIntensity = false);

    void SetColor(void* consoleHandle, Color foreground, Color background, bool foregroundIntensity = false, bool backgroundIntensity = false);

    void ClearScreen();
}