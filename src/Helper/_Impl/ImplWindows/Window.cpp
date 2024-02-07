#include "../../Windows/Window.h"
#include "../../Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include "../../String.h"

namespace Helper
{
    struct Window::WindowHandle
    {
        HWND hWnd;
    };

    struct Window::DeviceContextHandle
    {
        HDC hDeviceContext;
    };

    static LRESULT CALLBACK DefaultWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg) {
            case WM_CLOSE:
                ::PostQuitMessage(0);
                return 0;
            case WM_DESTROY:
                return 0;
            default:
                return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    void Window::Register(const std::string& windowRegisterName)
    {
        RegisterInfo info { nullptr, nullptr, nullptr, GetDefaultWinMsgProc() };
        Register(windowRegisterName, info);
    }

    void Window::Register(const std::string& windowRegisterName, RegisterInfo info)
    {
        const auto windowRegisterNameW = String::StringToWideString(windowRegisterName);

        const WNDCLASSEXW wc = {
            sizeof(wc),
            CS_CLASSDC,
            reinterpret_cast<WNDPROC>(info.pWinMsgProc),
            0L,
            0L,
            ::GetModuleHandle(nullptr),
            (info.pIcon->hIcon),
            (info.pCursor->hCursor),
            nullptr,
            nullptr,
            windowRegisterNameW.c_str(),
            (info.pIconSmall->hIcon),
        };

        ::RegisterClassExW(&wc);
    }

    ResPtr<Window::WindowHandle> Window::Show(const std::string& windowRegisterName, const std::string& windowTitleName, int width, int height)
    {
        StyleInfo menuStyle { true, true, true, true };
        return Show(windowRegisterName, windowTitleName, width, height, menuStyle);
    }

    ResPtr<Window::WindowHandle> Window::Show(const std::string& windowRegisterName, const std::string& windowTitleName, int width, int height, StyleInfo menuStyle, void* windowCreateData)
    {
        DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION;

        if (menuStyle.hasSysmenu)
        {
            windowStyle |= WS_SYSMENU;

            if (menuStyle.hasMaxBtn)
                windowStyle |= WS_MAXIMIZEBOX;

            if (menuStyle.hasMinBtn)
                windowStyle |= WS_MINIMIZEBOX;
        }

        if (menuStyle.canResize)
            windowStyle |= WS_THICKFRAME;

        const auto windowRegisterNameW = String::StringToWideString(windowRegisterName);
        const auto windowTitleNameW = String::StringToWideString(windowTitleName);

        ResPtr<WindowHandle> result(new WindowHandle());
        result->hWnd = ::CreateWindowW(
                        windowRegisterNameW.c_str(),
                        windowTitleNameW.c_str(),
                        windowStyle,
                        100,
                        100,
                        width,
                        height,
                        nullptr,
                        nullptr,
                        ::GetModuleHandle(nullptr),
                        windowCreateData);

        ::SetWindowTextW(result->hWnd, windowTitleNameW.c_str());
        ::ShowWindow(result->hWnd, SW_SHOWDEFAULT);
        ::UpdateWindow(result->hWnd);

        return result;
    }

    void Window::Destroy(ResPtr<WindowHandle>&& pWindowHandle)
    {
        ::DestroyWindow(pWindowHandle->hWnd);
    }

    void Window::UnRegister(const std::string& windowRegisterName)
    {
        const auto windowRegisterNameW = String::StringToWideString(windowRegisterName);
        ::UnregisterClassW(windowRegisterNameW.c_str(), GetModuleHandle(nullptr));
    }

    bool Window::MessageLoop(bool blockWhenNoWindowsMessage)
    {
        bool shouldQuit = false;
        MSG msg;

        if (blockWhenNoWindowsMessage)
        {
            // When GetMessage() retrieves a WM_QUIT message from the queue, it will return a value of 0
            if (::GetMessage(&msg, nullptr, 0U, 0U) == 0)
            {
                shouldQuit = true;
            }
            else
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
        else
        {
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);

                shouldQuit = msg.message == WM_QUIT;
                if (shouldQuit)
                    break;
            }
        }

        return shouldQuit;
    }

    void* Window::GetDefaultWinMsgProc()
    {
        return reinterpret_cast<void*>(&DefaultWindowProc);
    }

    ResPtr<Window::DeviceContextHandle> Window::GetDeviceContext(const ResPtr<WindowHandle>& pWindowHandle)
    {
        ResPtr<DeviceContextHandle> result(new DeviceContextHandle());
        result->hDeviceContext = ::GetDC(pWindowHandle->hWnd);
        return result;
    }

    void Window::ReleaseDeviceContext(const ResPtr<WindowHandle>& hWnd, ResPtr<DeviceContextHandle>&& hDeviceContext)
    {
        ::ReleaseDC(hWnd->hWnd, hDeviceContext->hDeviceContext);
    }

    void Window::DeviceContextSwapBuffer(const ResPtr<DeviceContextHandle>& hDeviceContext)
    {
        ::SwapBuffers(hDeviceContext->hDeviceContext);
    }

    void Window::ReleaseCapture()
    {
        ::ReleaseCapture();
    }

    void Window::SetCapture(const ResPtr<WindowHandle>& hWnd)
    {
        ::SetCapture(hWnd->hWnd);
    }

#ifndef DISABLE_HELPER_WINDOWS_OPENGL_SUPPORT

    struct Window::OpenGLRenderContextHandle
    {
        HGLRC hOpenGLRenderContext;
    };

    bool Window::PrepareWindowPixelFormat(const ResPtr<WindowHandle>& hWnd)
    {
        const PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
            PFD_TYPE_RGBA,        // The kind of framebuffer.
            32,                   // Color depth of the framebuffer.
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            24,
            8,
            0
        };

        const HDC hdc = ::GetDC(hWnd->hWnd);
        const unsigned int pixelFormat = ::ChoosePixelFormat(hdc, &pfd);

        return ::SetPixelFormat(hdc, pixelFormat, &pfd);;
    }

    ResPtr<Window::OpenGLRenderContextHandle> Window::CreateRenderContext(const ResPtr<DeviceContextHandle>& hDeviceContext)
    {
        ResPtr<OpenGLRenderContextHandle> result(new OpenGLRenderContextHandle());
        result->hOpenGLRenderContext = ::wglCreateContext(hDeviceContext->hDeviceContext);
        return result;
    }

    bool Window::BindRenderContext(const ResPtr<DeviceContextHandle>& hDeviceContext, const ResPtr<OpenGLRenderContextHandle>& hRenderContext)
    {
        return ::wglMakeCurrent(hDeviceContext->hDeviceContext, hRenderContext->hOpenGLRenderContext);
    }

    void Window::DestroyRenderContext(ResPtr<OpenGLRenderContextHandle>&& hRenderContext)
    {
        ::wglDeleteContext(hRenderContext->hOpenGLRenderContext);
    }

#endif
}

#endif