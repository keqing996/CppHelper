
#include <Infra/Windows//Window.h>
#include <Infra/Timer.h>

int main()
{
    Infra::Window win(800, 600, "Test");

    while (true)
        win.EventLoop();
}