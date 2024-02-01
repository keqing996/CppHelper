#pragma once

#include "../../PlatformDefine.h"

#include <string>
#include <memory>

namespace Helper::OS
{

#if PLATFORM_WINDOWS

    struct WindowHandle;
    struct DeviceContextHandle;

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
        static auto Register(const std::string& windowRegisterName) -> void;
        static auto Register(const std::string& windowRegisterName, RegisterInfo info) -> void;
        static auto Show(const std::string& windowRegisterName, const std::string& windowTitleName, int width,
                         int height) -> std::unique_ptr<WindowHandle>;
        static auto Show(const std::string& windowRegisterName, const std::string& windowTitleName, int width,
                         int height, StyleInfo menuStyle,
                         void* windowCreateData = nullptr) -> std::unique_ptr<WindowHandle>;
        static auto Destroy(const std::unique_ptr<WindowHandle>& pWindowHandle) -> void;
        static auto UnRegister(const std::string& windowRegisterName) -> void;

        static auto MessageLoop(bool blockWhenNoWindowsMessage = false) -> bool;
        static auto GetDefaultWinMsgProc() -> void*;
        static auto GetDeviceContext(const std::unique_ptr<WindowHandle>& pWindowHandle) -> std::unique_ptr<DeviceContextHandle>;
        static auto ReleaseDeviceContext(const std::unique_ptr<WindowHandle>& hWnd, const std::unique_ptr<DeviceContextHandle>& hDeviceContext) -> void;
        static auto DeviceContextSwapBuffer(const std::unique_ptr<DeviceContextHandle>& hDeviceContext) -> void;
    };

#endif

}