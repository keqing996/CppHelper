#include "../../Windows/Window.h"
#include "../../Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include "../../String.h"

namespace Infra
{
    struct Window::WindowHandle
    {
        HWND hWnd;
    };

    struct Window::DeviceContextHandle
    {
        HDC hDeviceContext;
    };


}

#endif