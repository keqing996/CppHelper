
#include <Helper/WinApi/WinApiWindow.h>
#include <Helper/Timer.h>

int main()
{
    Helper::Win::Window::RegisterInfo info {
        nullptr,
        nullptr,
        nullptr,
        Helper::Win::Window::GetDefaultWinMsgProc()
    };

    Helper::Win::Window::Register(L"Window", info);

    Helper::Win::Window::StyleInfo style {
        true,
        true,
        true,
        false
    };

    {
        void* hWnd = Helper::Win::Window::Show(L"Window", L"Title", 800, 600, style);

        while (true)
        {
            bool quit = Helper::Win::Window::MessageLoop();
            if (quit)
                break;
        }

        Helper::Win::Window::Destroy(hWnd);
    }

    style.hasMaxBtn = false;
    style.hasMinBtn = false;

    {
        void* hWnd = Helper::Win::Window::Show(L"Window", L"Title", 800, 600, style);

        while (true)
        {
            bool quit = Helper::Win::Window::MessageLoop();
            if (quit)
                break;
        }

        Helper::Win::Window::Destroy(hWnd);
    }

    style.hasSysmenu = false;

    {
        void* hWnd = Helper::Win::Window::Show(L"Window", L"Title", 800, 600, style);

        Helper::Timer<Helper::TimePrecision::Seconds> timer;
        timer.SetNow();
        while (true)
        {
            bool quit = Helper::Win::Window::MessageLoop();
            if (quit)
                break;

            if (timer.GetInterval() > 3)
                break;
        }

        Helper::Win::Window::Destroy(hWnd);
    }

    Helper::Win::Window::UnRegister(L"Window");
}