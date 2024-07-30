#pragma once

#include "Infra/PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "Infra/Network/SocketState.h"
#include "Infra/Platform/Windows/WindowsDefine.h"

#include <ws2tcpip.h>

namespace Infra
{
    using SocketHandle = SOCKET;

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