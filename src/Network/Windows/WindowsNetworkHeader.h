#pragma once

#include "Infra/PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "Infra/Platform/Windows/WindowsDefine.h"

#include <ws2tcpip.h>

using SocketHanle = SOCKET;

inline SocketHanle GetInvalidSocket()
{
    return INVALID_SOCKET;
}

#endif