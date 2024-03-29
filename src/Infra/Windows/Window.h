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

        class Event
        {

        };

    private:
        Window();

    public:
        void WindowEventProcess(uint32_t message, void* wpara, void* lpara);

    private:
        WindowHandle _hWindow = nullptr;

    private:
        static void RegisterWindowClass();
        static void UnRegisterWindowClass();

    private:
        inline static int _sGlobalWindowsCount = 0;
        inline static const wchar_t* _sWindowRegisterName = L"InfraWindow";


    };
}

#endif