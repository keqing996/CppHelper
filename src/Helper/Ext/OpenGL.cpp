#include "OpenGL.h"
#include "../WinApi/WindowsPlatform.h"

#ifdef HELPER_EXT_OPENGL

namespace Helper::Ext::OpenGL
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

#endif