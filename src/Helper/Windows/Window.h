#pragma once

#include "../PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "Resource.h"

#include <string>
#include <memory>

namespace Helper::OS
{

    struct WindowHandle;
    struct DeviceContextHandle;

#if ENABLE_HELPER_EXT_OPENGL
    struct OpenGLRenderContextHandle;
#endif

    struct RegisterInfo
    {
        IconResource* pIcon;
        IconResource* pIconSmall;
        CursorResource* pCursor;
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

#if ENABLE_HELPER_EXT_OPENGL
    public: // OpenGL Ext
        static auto PrepareWindowPixelFormat(const std::unique_ptr<WindowHandle>& hWnd) -> bool;
        static auto CreateRenderContext(const std::unique_ptr<DeviceContextHandle>& hDeviceContext) -> std::unique_ptr<OpenGLRenderContextHandle>;
        static auto BindRenderContext(const std::unique_ptr<DeviceContextHandle>& hDeviceContext, const std::unique_ptr<OpenGLRenderContextHandle>& hRenderContext) -> bool;
        static auto DestroyRenderContext(const std::unique_ptr<OpenGLRenderContextHandle>& hRenderContext) -> void;

#endif
    };
}

#endif