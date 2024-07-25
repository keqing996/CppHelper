#include "Infra/PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "WindowsNetworkHeader.h"

#pragma comment(lib, "Ws2_32.lib")

struct WinSocketGuard
{
    WinSocketGuard()
    {
        WSADATA init;
        ::WSAStartup(MAKEWORD(2, 2), &init);
    }

    ~WinSocketGuard()
    {
        ::WSACleanup();
    }
};

WinSocketGuard gWinSocketGuard;

#endif

