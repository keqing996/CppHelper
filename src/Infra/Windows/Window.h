#pragma once

#include "../PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "WindowEvent.h"
#include <cstdint>
#include <string>
#include <memory>

namespace Infra
{
    enum class WindowStyle: int
    {
        None = 0,
        HaveTitleBar = 1 << 0,
        HaveResize = 1 << 1,
        HaveClose = 1 << 2,
        Default = HaveTitleBar | HaveResize | HaveClose
    };

    class Window
    {
    public:
        using WindowHandle = void*;
        using DeviceContextHandle = void*;
        using IconHandle = void*;

    private:
        Window(int width, int height, const std::string& title, WindowStyle style = WindowStyle::Default);
        ~Window();

    public:
        auto WindowEventProcess(uint32_t message, void* wpara, void* lpara) -> void;
        auto SetSize(int width, int height) -> void;
        auto GetSystemHandle() -> void*;
        auto SetIcon(unsigned int width, unsigned int height, const std::byte* pixels) -> void;
        auto SetWindowVisible(bool show) -> void;
        auto SetCursorVisible(bool show) -> void;
        auto SetCursorCapture(bool capture) -> void;


    private:
        // Window handle
        WindowHandle _hWindow;

        // State
        std::pair<int, int> _windowSize;
        bool _enableKeyRepeat;
        bool _cursorVisible;
        bool _cursorCapture;

        // Resource
        IconHandle _hIcon;


    private:
        static void RegisterWindowClass();
        static void UnRegisterWindowClass();

    private:
        inline static int _sGlobalWindowsCount = 0;
        inline static const wchar_t* _sWindowRegisterName = L"InfraWindow";


    };
}

#endif