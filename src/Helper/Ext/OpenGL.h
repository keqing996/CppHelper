#pragma once

// Cmake: find_package(OpenGL REQUIRED)
// Link: ${OPENGL_LIBRARIES}
// Windows Kits\10\Lib\10.0.xxx.0\um\x64\OpenGL32.Lib

#ifdef HELPER_EXT_OPENGL

namespace Helper::Ext::OpenGL
{
    bool PrepareWindowPixelFormat(void* hWnd);

    void* CreateRenderContext(void* hDeviceContext);

    bool BindRenderContext(void* hDeviceContext, void* hRenderContext);

    void DestroyRenderContext(void* hRenderContext);
}

#endif