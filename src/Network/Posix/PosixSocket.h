#pragma once

#include "Infra/PlatformDefine.h"

#if PLATFORM_SUPPORT_POSIX

#include "Infra/Network/SocketState.h"

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

namespace Infra
{
    using SocketHandle = int;

    namespace Device
    {
        INFRA_FORCE_INLINE
        SocketHandle ToNativeHandle(void* handle);

        INFRA_FORCE_INLINE
        void* ToGeneralHandle(SocketHandle sock);
        
        SocketHandle GetInvalidSocket();

        void CloseSocket(void* handle);

        bool SetSocketBlocking(void* handle, bool block);
    }
}

#endif