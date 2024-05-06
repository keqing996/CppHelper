#pragma once

#include <string>

namespace ImGuiApp
{
    struct WindowStyle
    {
        bool hasMaxBox = true;
        bool hasMinBox = true;
        bool canResize = true;
        bool blockWhenNoWindowsMessage = false;
    };

    class Window: NonCopyable
    {
    public:
        Window(const std::string& title, int width, int height);
        Window(const std::string& title, int width, int height, WindowStyle windowStyle);


    public:
        // Windows window
        void Show();


    private:
        // Windows window
        std::string _windowRegisterName = "DefaultImGuiWindow";
        std::string _windowTitleName = "ImGuiWindow";
        int _windowWidth;
        int _windowHeight;
        WindowStyle _windowStyle;


    };
}