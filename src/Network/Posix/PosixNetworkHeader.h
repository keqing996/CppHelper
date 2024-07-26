#pragma once

#include "Infra/PlatformDefine.h"

#if PLATFORM_SUPPORT_POSIX

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

using SocketHanle = int;

inline SocketHanle GetInvalidSocket()
{
    return 0;
}

#endif