#include "Infra/Platform/Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include "Infra/Network/Socket.h"
#include "WindowsNetworkHeader.h"

namespace Infra
{
    bool Socket::SetBlocking(bool block)
    {
        u_long blocking = block ? 0 : 1;
        auto ret = ::ioctlsocket(ToNativeHandle(_handle), static_cast<long>(FIONBIO), &blocking);
        return ret == 0;
    }

    void Socket::Close()
    {
        ::closesocket(ToNativeHandle(_handle));
    }
}

#endif
