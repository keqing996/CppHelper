#pragma once

#include "../PlatformDefine.h"

#include <string>
#include <memory>

namespace Helper::OS
{

#if PLATFORM_WINDOWS



    struct RegisterInfo
    {
        void* hIcon;
        void* hIconSmall;
        void* hCursor;
        void* pWinMsgProc;
    };

    struct StyleInfo
    {
        bool hasSysmenu;
        bool hasMaxBtn;
        bool hasMinBtn;
        bool canResize;
    };

    class Window
    {
    public:
        Window() = delete;

    public:
        static void Register(const std::wstring& windowRegisterName);
        static void Register(const std::wstring& windowRegisterName, RegisterInfo info);
        static void* Show(const std::wstring& windowRegisterName, const std::wstring& windowTitleName, int width, int height);
        static void* Show(const std::wstring& windowRegisterName, const std::wstring& windowTitleName, int width, int height, StyleInfo menuStyle, void* windowCreateData = nullptr);
        static void Destroy(void* hWnd);
        static void UnRegister(const std::wstring& windowRegisterName);
        static bool MessageLoop(bool blockWhenNoWindowsMessage = false);
        static void* GetDefaultWinMsgProc();
        static void* GetDeviceContext(void* hWnd);
        static void ReleaseDeviceContext(void* hWnd, void* hDeviceContext);
        static void DeviceContextSwapBuffer(void* hDeviceContext);
    };

#endif

}