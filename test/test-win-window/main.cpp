
#include <Infra/WinApi/WinApiWindow.h>
#include <Infra/Timer.h>

int main()
{
    Infra::Win::Window::RegisterInfo info {
        nullptr,
        nullptr,
        nullptr,
        Infra::Win::Window::GetDefaultWinMsgProc()
    };

    Infra::Win::Window::Register(L"Window", info);

    Infra::Win::Window::StyleInfo style {
        true,
        true,
        true,
        false
    };

    {
        void* hWnd = Infra::Win::Window::Show(L"Window", L"Title", 800, 600, style);

        while (true)
        {
            bool quit = Infra::Win::Window::MessageLoop();
            if (quit)
                break;
        }

        Infra::Win::Window::Destroy(hWnd);
    }

    style.hasMaxBtn = false;
    style.hasMinBtn = false;

    {
        void* hWnd = Infra::Win::Window::Show(L"Window", L"Title", 800, 600, style);

        while (true)
        {
            bool quit = Infra::Win::Window::MessageLoop();
            if (quit)
                break;
        }

        Infra::Win::Window::Destroy(hWnd);
    }

    style.hasSysmenu = false;

    {
        void* hWnd = Infra::Win::Window::Show(L"Window", L"Title", 800, 600, style);

        Infra::Timer<Infra::TimePrecision::Seconds> timer;
        timer.SetNow();
        while (true)
        {
            bool quit = Infra::Win::Window::MessageLoop();
            if (quit)
                break;

            if (timer.GetInterval() > 3)
                break;
        }

        Infra::Win::Window::Destroy(hWnd);
    }

    Infra::Win::Window::UnRegister(L"Window");
}