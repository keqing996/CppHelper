#include "WindowsPlatform.h"
#include "WinApiWindow.h"

namespace Helper::Win::Window
{
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

    void Register(const std::wstring& windowRegisterName)
    {
        RegisterInfo info { nullptr, nullptr, nullptr, GetDefaultWinMsgProc() };
        Register(windowRegisterName, info);
    }

    void Register(const std::wstring& windowRegisterName, RegisterInfo info)
    {
        const WNDCLASSEXW wc = {
            sizeof(wc),
            CS_CLASSDC,
            reinterpret_cast<WNDPROC>(info.pWinMsgProc),
            0L,
            0L,
            ::GetModuleHandle(nullptr),
            static_cast<HICON>(info.hIcon),
            static_cast<HICON>(info.hCursor),
            nullptr,
            nullptr,
            windowRegisterName.c_str(),
            static_cast<HICON>(info.hIconSmall),
        };

        ::RegisterClassExW(&wc);
    }

    void* Show(const std::wstring& windowRegisterName, const std::wstring& windowTitleName, int width, int height)
    {
        StyleInfo menuStyle { true, true, true, true };
        return Show(windowRegisterName, windowTitleName, width, height, menuStyle);
    }

    void* Show(const std::wstring& windowRegisterName, const std::wstring& windowTitleName, int width, int height, StyleInfo menuStyle, void* windowCreateData)
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

        HWND hWnd = ::CreateWindowW(
                        windowRegisterName.c_str(),
                        windowTitleName.c_str(),
                        windowStyle,
                        100,
                        100,
                        width,
                        height,
                        nullptr,
                        nullptr,
                        ::GetModuleHandle(nullptr),
                        windowCreateData);

        ::SetWindowTextW(hWnd, windowTitleName.c_str());
        ::ShowWindow(hWnd, SW_SHOWDEFAULT);
        ::UpdateWindow(hWnd);

        return (void*)hWnd;
    }

    void Destroy(void* hWnd)
    {
        ::DestroyWindow(static_cast<HWND>(hWnd));
    }

    void UnRegister(const std::wstring& windowRegisterName)
    {
        ::UnregisterClassW(windowRegisterName.c_str(), GetModuleHandle(nullptr));
    }

    bool MessageLoop(bool blockWhenNoWindowsMessage)
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

    void* GetDefaultWinMsgProc()
    {
        return reinterpret_cast<void*>(&DefaultWindowProc);
    }

    void* GetDeviceContext(void* hWnd)
    {
        return ::GetDC(static_cast<HWND>(hWnd));
    }

    void ReleaseDeviceContext(void* hWnd, void* hDeviceContext)
    {
        ::ReleaseDC(static_cast<HWND>(hWnd), static_cast<HDC>(hDeviceContext));
    }

    void DeviceContextSwapBuffer(void* hDeviceContext)
    {
        ::SwapBuffers(static_cast<HDC>(hDeviceContext));
    }

    namespace OpenGL
    {

        bool PrepareWindowPixelFormat(void* hWnd)
        {
            PIXELFORMATDESCRIPTOR pfd =
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

            HDC hdc = ::GetDC(static_cast<HWND>(hWnd));
            unsigned int pixelFormat = ::ChoosePixelFormat(hdc, &pfd);

            return ::SetPixelFormat(hdc, pixelFormat, &pfd);;
        }

        void* CreateRenderContext(void* hDeviceContext)
        {
            return ::wglCreateContext(static_cast<HDC>(hDeviceContext));
        }

        bool BindRenderContext(void* hDeviceContext, void* hRenderContext)
        {
            return ::wglMakeCurrent(static_cast<HDC>(hDeviceContext), static_cast<HGLRC>(hRenderContext));
        }

        void DestroyRenderContext(void* hRenderContext)
        {
            ::wglDeleteContext(static_cast<HGLRC>(hRenderContext));
        }

    }
}