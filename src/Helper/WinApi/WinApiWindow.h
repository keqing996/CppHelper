#pragma once

#include <string>

namespace Helper::Win::Window
{
    struct RegisterInfo
    {
        void* hIcon;
        void* hIconSmall;
        void* hCursor;
        void* pWinMsgProc;
    };

    struct StyleInfo
    {
        bool hasSysmenu;
        bool hasMaxBtn;
        bool hasMinBtn;
        bool canResize;
    };

    void Register(const std::wstring& windowRegisterName, RegisterInfo info);

    void* Show(const std::wstring& windowRegisterName, const std::wstring& windowTitleName, int width, int height, StyleInfo menuStyle, void* windowCreateData = nullptr);

    void Destroy(void* hWnd);

    void UnRegister(const std::wstring& windowRegisterName);

    bool MessageLoop(bool blockWhenNoWindowsMessage = false);

    void* GetDefaultWinMsgProc();

    void* CreateDeviceContext(void* hWnd);

    void DestroyDeviceContext(void* hWnd, void* hDeviceContext);

    void DeviceContextSwapBuffer(void* hDeviceContext);

    namespace OpenGL
    {
        bool PrepareWindowPixelFormat(void* hWnd);

        void* CreateRenderContext(void* hDeviceContext);

        bool BindRenderContext(void* hDeviceContext, void* hRenderContext);

        void DestroyRenderContext(void* hRenderContext);

    }

}