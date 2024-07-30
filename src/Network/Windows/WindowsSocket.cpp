#include "WindowsSocket.h"

#if PLATFORM_WINDOWS

#pragma comment(lib, "Ws2_32.lib")

namespace Infra::Device
{
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

    SocketHandle ToNativeHandle(void* handle)
    {
        return reinterpret_cast<SocketHandle>(handle);
    }

    void* ToGeneralHandle(SocketHandle sock)
    {
        return reinterpret_cast<void*>(sock);
    }

    SocketHandle GetInvalidSocket()
    {
        return INVALID_SOCKET;
    }

    void CloseSocket(void* handle)
    {
        ::closesocket(ToNativeHandle(handle));
    }

    bool SetSocketBlocking(void* handle, bool block)
    {
        u_long blocking = block ? 0 : 1;
        auto ret = ::ioctlsocket(ToNativeHandle(handle), static_cast<long>(FIONBIO), &blocking);
        return ret == 0;
    }
}


#endif