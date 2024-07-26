#pragma once

#include "Infra/PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "Infra/Platform/Windows/WindowsDefine.h"

#include <ws2tcpip.h>

inline SOCKET GetInvalidSocket()
{
    return INVALID_SOCKET;
}

#define ToGeneralHandle(x) reinterpret_cast<void*>(x)
#define ToNativeHandle(x) reinterpret_cast<SOCKET>(x)

#endif