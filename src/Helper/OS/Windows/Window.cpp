#include "Window.h"
#include "WindowsDefine.h"

#if PLATFORM_WINDOWS

#include "../../String.h"

namespace Helper::OS
{
    struct WindowHandle
    {
        HWND hWnd;
    };

    struct DeviceContextHandle
    {
        HDC hDC;
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

    std::unique_ptr<WindowHandle> Window::Show(const std::string& windowRegisterName, const std::string& windowTitleName, int width, int height)
    {
        StyleInfo menuStyle { true, true, true, true };
        return Show(windowRegisterName, windowTitleName, width, height, menuStyle);
    }

    std::unique_ptr<WindowHandle> Window::Show(const std::string& windowRegisterName, const std::string& windowTitleName, int width, int height, StyleInfo menuStyle, void* windowCreateData)
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

        std::unique_ptr<WindowHandle> result(new WindowHandle());
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

    void Window::Destroy(const std::unique_ptr<WindowHandle>& pWindowHandle)
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

    std::unique_ptr<DeviceContextHandle> Window::GetDeviceContext(const std::unique_ptr<WindowHandle>& pWindowHandle)
    {
        std::unique_ptr<DeviceContextHandle> result(new DeviceContextHandle());
        result->hDC = ::GetDC(pWindowHandle->hWnd);
        return result;
    }

    void Window::ReleaseDeviceContext(const std::unique_ptr<WindowHandle>& hWnd, const std::unique_ptr<DeviceContextHandle>& hDeviceContext)
    {
        ::ReleaseDC(hWnd->hWnd, hDeviceContext->hDC);
    }

    void Window::DeviceContextSwapBuffer(const std::unique_ptr<DeviceContextHandle>& hDeviceContext)
    {
        ::SwapBuffers(hDeviceContext->hDC);
    }
}

#endif