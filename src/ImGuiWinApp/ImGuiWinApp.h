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
        enum class TickStage
        {
            PreFrame,
            OnFrame,
            PostFrame,
        };

    public:
        ImGuiWinApp(int width, int height, const std::string& title, int style = (int)WindowStyle::Default);
        ~ImGuiWinApp();

    public:
        void SetOnEventHandler(const std::function<bool(const WindowEvent&, bool*)>& handler);
        void AppLoop();
        void EnableVSync(bool enable);
        void CloseWindow();
        Window& GetNativeWindow();

        // Font
        int GetNormalFontSize();
        int GetLargeFontSize();
        ImFont* CreateImGuiFont(void* fontData, int fontDataSize, int fontSize, bool transferDataOwnership = true);
        ImFont* CreateImGuiFont(const std::string& ttfPath, int fontSize);
        ImFont* GetFontRegularNormal() const;
        ImFont* GetFontRegularLarge() const;
        ImFont* GetFontBoldNormal() const;
        ImFont* GetFontBoldLarge() const;

    public:
        template<TickStage stage>
        void SetTickFunction(const std::function<void(ImGuiWinApp&)>& tickFunction)
        {
            if constexpr (stage == TickStage::PreFrame)
                _preFrameTick = tickFunction;
            else if constexpr (stage == TickStage::OnFrame)
                _onFrameTick = tickFunction;
            else if constexpr (stage == TickStage::PostFrame)
                _postFrameTick = tickFunction;
        }

    private:
        bool D3d11SetUp();
        void D3d11Clear();
        bool ImGuiSetUp();
        void ImGuiClear();
        void DefaultOnEventHandler(const WindowEvent& e, bool* breakAppLoop);

    private:
        // Window
        Window _window;
        std::function<bool(const WindowEvent&, bool*)> _onEventHandler = nullptr;

        // D3d11
        ID3D11Device* _pD3dDevice = nullptr;
        ID3D11DeviceContext* _pD3dDeviceContext = nullptr;
        IDXGISwapChain* _pSwapChain = nullptr;
        ID3D11RenderTargetView* _pMainRenderTargetView = nullptr;

        // Option
        bool _enableVSync = true;

        // Updater
        std::function<void(ImGuiWinApp&)> _preFrameTick = nullptr;
        std::function<void(ImGuiWinApp&)> _onFrameTick = nullptr;
        std::function<void(ImGuiWinApp&)> _postFrameTick = nullptr;

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