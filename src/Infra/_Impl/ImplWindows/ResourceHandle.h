#pragma once

#include "../../PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "../../Windows/WindowsDefine.h"

namespace Helper
{
    struct DataResource
    {
        void* pData;
        DWORD size;
    };

    struct CursorResource
    {
        HCURSOR hCursor;
    };

    struct BitmapResource
    {
        HBITMAP hBitmap;
    };

    struct IconResource
    {
        HICON hIcon;
    };
}

#endif