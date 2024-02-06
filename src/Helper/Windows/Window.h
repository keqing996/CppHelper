#pragma once

#include "../PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "Resource.h"
#include "../ResourcePtr.h"

#include <string>
#include <memory>

namespace Helper
{
    class Window
    {
    public:
        Window() = delete;

        struct WindowHandle;
        struct DeviceContextHandle;

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

    public:
        static auto Register(const std::string& windowRegisterName) -> void;
        static auto Register(const std::string& windowRegisterName, RegisterInfo info) -> void;
        static auto Show(const std::string& windowRegisterName, const std::string& windowTitleName, int width,
                         int height) -> ResPtr<WindowHandle>;
        static auto Show(const std::string& windowRegisterName, const std::string& windowTitleName, int width,
                         int height, StyleInfo menuStyle,
                         void* windowCreateData = nullptr) -> ResPtr<WindowHandle>;
        static auto Destroy(ResPtr<WindowHandle>&& pWindowHandle) -> void;
        static auto UnRegister(const std::string& windowRegisterName) -> void;

        static auto MessageLoop(bool blockWhenNoWindowsMessage = false) -> bool;
        static auto GetDefaultWinMsgProc() -> void*;
        static auto GetDeviceContext(const ResPtr<WindowHandle>& pWindowHandle) -> ResPtr<DeviceContextHandle>;
        static auto ReleaseDeviceContext(const ResPtr<WindowHandle>& hWnd, ResPtr<DeviceContextHandle>&& hDeviceContext) -> void;
        static auto DeviceContextSwapBuffer(const ResPtr<DeviceContextHandle>& hDeviceContext) -> void;

        static auto SetCapture(const ResPtr<WindowHandle>& hWnd) -> void;
        static auto ReleaseCapture() -> void;

#ifndef DISABLE_HELPER_WINDOWS_OPENGL_SUPPORT
    public: // OpenGL Ext
        struct OpenGLRenderContextHandle;

        static auto PrepareWindowPixelFormat(const ResPtr<WindowHandle>& hWnd) -> bool;
        static auto CreateRenderContext(const ResPtr<DeviceContextHandle>& hDeviceContext) -> ResPtr<OpenGLRenderContextHandle>;
        static auto BindRenderContext(const ResPtr<DeviceContextHandle>& hDeviceContext, const ResPtr<OpenGLRenderContextHandle>& hRenderContext) -> bool;
        static auto DestroyRenderContext(ResPtr<OpenGLRenderContextHandle>&& hRenderContext) -> void;

#endif
    };
}

#endif