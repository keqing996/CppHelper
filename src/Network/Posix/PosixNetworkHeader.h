#pragma once

#include "Infra/PlatformDefine.h"

#if PLATFORM_SUPPORT_POSIX

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

inline int GetInvalidSocket()
{
    return 0;
}

#define ToGeneralHandle(x) reinterpret_cast<void*>(x)
#define ToNativeHandle(x) reinterpret_cast<int>(x)

#endif