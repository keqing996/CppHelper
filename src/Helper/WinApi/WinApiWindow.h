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

    void Register(const std::wstring& windowRegisterName);

    void Register(const std::wstring& windowRegisterName, RegisterInfo info);

    void* Show(const std::wstring& windowRegisterName, const std::wstring& windowTitleName, int width, int height);

    void* Show(const std::wstring& windowRegisterName, const std::wstring& windowTitleName, int width, int height, StyleInfo menuStyle, void* windowCreateData = nullptr);

    void Destroy(void* hWnd);

    void UnRegister(const std::wstring& windowRegisterName);

    bool MessageLoop(bool blockWhenNoWindowsMessage = false);

    void* GetDefaultWinMsgProc();

    void* GetDeviceContext(void* hWnd);

    void ReleaseDeviceContext(void* hWnd, void* hDeviceContext);

    void DeviceContextSwapBuffer(void* hDeviceContext);

}