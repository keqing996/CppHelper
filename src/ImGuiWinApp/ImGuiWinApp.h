#pragma once

#include <string>
#include <d3d11.h>
#include <Infra/NonCopyable.hpp>
#include <NativeWinApp/Window.h>

namespace Infra
{
    class ImGuiWinApp: NonCopyable
    {
    public:
        ImGuiWinApp(int width, int height, const std::string& title, int style = (int)WindowStyle::Default);
        virtual ~ImGuiWinApp();

    public:
        bool IsCreateReady();

    private:
        bool D3d11SetUp();
        void D3d11Clear();
        bool ImGuiSetUp();
        void ImGuiClear();

    private:
        bool _ready = false;

        // Window
        Window _window;

        // D3d11
        ID3D11Device* _pD3dDevice = nullptr;
        ID3D11DeviceContext* _pD3dDeviceContext = nullptr;
        IDXGISwapChain* _pSwapChain = nullptr;
        ID3D11RenderTargetView* _pMainRenderTargetView = nullptr;
    };
}