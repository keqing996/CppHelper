#include <Windows.h>
#include <imgui.h>
#include "ImGuiWinApp.h"
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx11.h>

namespace Infra
{
    ImGuiWinApp::ImGuiWinApp(int width, int height, const std::string& title, int style)
        : _window(width, height, title, style)
    {
        bool d3dReady = D3d11SetUp();
        if (!d3dReady)
            return;

        ImGuiSetUp();
    }

    bool ImGuiWinApp::D3d11SetUp()
    {
        HWND hWnd = (HWND)_window.GetSystemHandle();

        // Setup swap chain
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = 0;
        //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
        HRESULT res = ::D3D11CreateDeviceAndSwapChain(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                createDeviceFlags,
                featureLevelArray,
                2,
                D3D11_SDK_VERSION,
                &sd,
                &_pSwapChain,
                &_pD3dDevice,
                &featureLevel,
                &_pD3dDeviceContext
                );

        if (res != S_OK)
            return false;

        // Render target
        ID3D11Texture2D* pBackBuffer;
        _pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        _pD3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pMainRenderTargetView);
        pBackBuffer->Release();

        return true;
    }

    void ImGuiWinApp::D3d11Clear()
    {
        _ready = false;

        if (_pMainRenderTargetView)
        {
            _pMainRenderTargetView->Release();
            _pMainRenderTargetView = nullptr;
        }

        if (_pSwapChain)
        {
            _pSwapChain->Release();
            _pSwapChain = nullptr;
        }

        if (_pD3dDeviceContext)
        {
            _pD3dDeviceContext->Release();
            _pD3dDeviceContext = nullptr;
        }

        if (_pD3dDevice)
        {
            _pD3dDevice->Release();
            _pD3dDevice = nullptr;
        }
    }

    bool ImGuiWinApp::IsCreateReady()
    {
        return _ready;
    }

    ImGuiWinApp::~ImGuiWinApp()
    {
        ImGuiClear();
        D3d11Clear();
    }

    bool ImGuiWinApp::ImGuiSetUp()
    {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init((HWND)_window.GetSystemHandle());
        ImGui_ImplDX11_Init(_pD3dDevice, _pD3dDeviceContext);

        return true;
    }

    void ImGuiWinApp::ImGuiClear()
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
}