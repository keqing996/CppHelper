#pragma once

#include <string>
#include <memory>
#include <functional>
#include <d3d11.h>
#include <Infra/NonCopyable.hpp>
#include <NativeWinApp/Window.h>

class ImFont;
class ImFontAtlas;

namespace Infra
{
    class ImGuiWinApp final : NonCopyable
    {
    public:
        ImGuiWinApp(int width, int height, const std::string& title, int style = (int)WindowStyle::Default);
        ~ImGuiWinApp();

    public:
        bool IsCreateReady();
        void AppLoop();
        void EnableVSync(bool enable);
        void SetViewUpdater(const std::function<void()>& updater);

        // Font
        ImFont* CreateImGuiFont(void* fontData, int fontDataSize, int fontSize, bool transferDataOwnership = true);
        ImFont* GetFontRegularNormal();
        ImFont* GetFontRegularLarge();
        ImFont* GetFontBoldNormal();
        ImFont* GetFontBoldLarge();

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

        // Option
        bool _enableVSync = true;

        // Updater
        std::function<void()> _viewUpdater = nullptr;

        // Font
        static constexpr int NORMAL_FONT_SIZE = 16;
        static constexpr int LARGE_FONT_SIZE = 20;
        std::unique_ptr<ImFontAtlas> _pSharedImGuiFonts = nullptr;
        ImFont* _pFontRegularNormal = nullptr;
        ImFont* _pFontRegularLarge = nullptr;
        ImFont* _pFontBoldNormal = nullptr;
        ImFont* _pFontBoldLarge = nullptr;
    };
}