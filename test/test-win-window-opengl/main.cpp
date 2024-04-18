
#include <string>
#include <Infra/Windows/Window.h>

int main()
{
    Infra::Window window(800, 600, "TestOpengl");
    window.CreateOpenGLContext();

    while (true)
    {
        window.EventLoop();

        bool shouldWindowClose = false;
        while (window.HasEvent())
        {
            auto event = window.PopEvent();
            if (event.type == Infra::WindowEvent::Type::Close)
            {
                shouldWindowClose = true;
                break;
            }
        }

        if (shouldWindowClose)
            break;


        window.SwapBuffer();
    }

    return 0;
}