#pragma once

#include "../../PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "../../Windows/WindowsDefine.h"

namespace Helper
{
    struct ProcessHandle
    {
        HANDLE handle;
    };
}

#endif