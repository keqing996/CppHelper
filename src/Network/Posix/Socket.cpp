#include "Infra/PlatformDefine.h"

#if PLATFORM_SUPPORT_POSIX

#include "Infra/Network/Socket.h"
#include "PosixNetworkHeader.h"

namespace Infra
{
    bool Socket::SetBlocking(bool block)
    {
        int sock = ToNativeHandle(_handle);
        const int status = ::fcntl(sock, F_GETFL);
        if (block)
        {
            int ret = ::fcntl(sock, F_SETFL, status & ~O_NONBLOCK);
            if (ret == -1)
                return false;
        }
        else
        {
            int ret = ::fcntl(sock, F_SETFL, status | O_NONBLOCK);
            if (ret == -1)
                return false;
        }

        return true;
    }

    void Socket::Close()
    {
        ::close(ToNativeHandle(_handle));
    }
}

#endif