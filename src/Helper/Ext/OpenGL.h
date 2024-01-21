#pragma once

#ifdef HELPER_EXT_OPENGL

namespace Helper::Ext::OpenGL
{
    bool PrepareWindowPixelFormat(void* hWnd);

    void* CreateRenderContext(void* hDeviceContext);

    bool BindRenderContext(void* hDeviceContext, void* hRenderContext);

    void DestroyRenderContext(void* hRenderContext);
}

#endif