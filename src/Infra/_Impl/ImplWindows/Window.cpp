#include "../../Windows/Window.h"
#include "../../Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include "../../ScopeGuard.h"
#include "../../String.h"
#include "../../Logger.h"
#include "../../Windows/Keyboard.h"
#include "../../Windows/Mouse.h"

namespace Infra
{
    class Support
    {
    public:
        Support() = delete;

    public:
        static bool FixProcessDpi()
        {
            // Try SetProcessDpiAwareness first
            if (FixProcessDpiBySetProcessDpiAwareness())
                return true;

            // Fall back to SetProcessDPIAware
            if (FixProcessDpiBySetProcessDPIAware())
                return true;

            return false;
        }

        static LRESULT CALLBACK DefaultWndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
        {
            // Associate handle and Window instance when the creation message is received
            if (message == WM_CREATE)
            {
                // Get WindowImplWin32 instance (it was passed as the last argument of CreateWindow)
                LONG_PTR window = reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

                // Set as the "user data" parameter of the window
                ::SetWindowLongPtrW(handle, GWLP_USERDATA, window);
            }

            // Get the WindowImpl instance corresponding to the window handle
            Window* window = handle ? reinterpret_cast<Window*>(::GetWindowLongPtrW(handle, GWLP_USERDATA)) : nullptr;

            // Forward the event to the appropriate function
            if (window)
            {
                window->WindowEventProcess(message, reinterpret_cast<void*>(wParam), reinterpret_cast<void*>(lParam));

                if (window->m_callback)
                    return ::CallWindowProcW(reinterpret_cast<WNDPROC>(window->m_callback), handle, message, wParam, lParam);
            }

            // We don't forward the WM_CLOSE message to prevent the OS from automatically destroying the window
            if (message == WM_CLOSE)
                return 0;

            // Don't forward the menu system command, so that pressing ALT or F10 doesn't steal the focus
            if ((message == WM_SYSCOMMAND) && (wParam == SC_KEYMENU))
                return 0;

            return ::DefWindowProcW(handle, message, wParam, lParam);
        }

    private:
        static bool FixProcessDpiBySetProcessDpiAwareness()
        {
            HINSTANCE pShcodeDll = ::LoadLibraryW(L"Shcore.dll");

            if (pShcodeDll == nullptr)
                return false;

            ScopeGuard pShcodeDllReleaseGuard = [&pShcodeDll]()
            {
                ::FreeLibrary(pShcodeDll);
            };

            void* pFuncSetProcessDpiAwareness = reinterpret_cast<void*>(GetProcAddress(pShcodeDll, "SetProcessDpiAwareness"));
            if (pFuncSetProcessDpiAwareness == nullptr)
                return false;

            enum ProcessDpiAwareness
            {
                ProcessDpiUnaware         = 0,
                ProcessSystemDpiAware     = 1,
                ProcessPerMonitorDpiAware = 2
            };

            using SetProcessDpiAwarenessFuncType = HRESULT (WINAPI*)(ProcessDpiAwareness);

            auto setProcessDpiAwarenessFunc = reinterpret_cast<SetProcessDpiAwarenessFuncType>(pFuncSetProcessDpiAwareness);
            if (setProcessDpiAwarenessFunc(ProcessPerMonitorDpiAware) == E_INVALIDARG)
                return false;

            return true;
        }

        static bool FixProcessDpiBySetProcessDPIAware()
        {
            HINSTANCE pUser32Dll = ::LoadLibraryW(L"user32.dll");

            if (pUser32Dll == nullptr)
                return false;

            ScopeGuard pShcodeDllReleaseGuard = [&pUser32Dll]()
            {
                ::FreeLibrary(pUser32Dll);
            };

            void* pFuncSetProcessDPIAware = reinterpret_cast<void*>(GetProcAddress(pUser32Dll, "SetProcessDPIAware"));
            if (pFuncSetProcessDPIAware == nullptr)
                return false;

            using SetProcessDPIAwareFuncType = BOOL (WINAPI*)(void);

            auto setProcessDpiAwareFunc = reinterpret_cast<SetProcessDPIAwareFuncType>(pFuncSetProcessDPIAware);
            if (!setProcessDpiAwareFunc())
                return false;

            return true;
        }
    };

    void Window::RegisterWindowClass()
    {
        WNDCLASSW windowClass;
        windowClass.style         = 0;
        windowClass.lpfnWndProc   = &Support::DefaultWndProc;
        windowClass.cbClsExtra    = 0;
        windowClass.cbWndExtra    = 0;
        windowClass.hInstance     = ::GetModuleHandleW(nullptr);
        windowClass.hIcon         = nullptr;
        windowClass.hCursor       = nullptr;
        windowClass.hbrBackground = nullptr;
        windowClass.lpszMenuName  = nullptr;
        windowClass.lpszClassName = _sWindowRegisterName;
        ::RegisterClassW(&windowClass);
    }

    void Window::UnRegisterWindowClass()
    {
        ::UnregisterClassW(_sWindowRegisterName, GetModuleHandleW(nullptr));
    }

}

#endif