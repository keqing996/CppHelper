#include "Window.h"

namespace ImGuiApp
{

    Window::Window(const std::string& title, int width, int height)
        : Window(title, width, height, WindowStyle())
    {
    }

    Window::Window(const std::string& title, int width, int height, WindowStyle windowStyle)
        : _windowTitleName(title)
        , _windowWidth(width)
        , _windowHeight(height)
    {
    }
}