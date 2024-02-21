#pragma once

#include "../../PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "../../Windows/WindowsDefine.h"

namespace Infra
{
    struct ProcessHandle
    {
        HANDLE handle;
    };
}

#endif