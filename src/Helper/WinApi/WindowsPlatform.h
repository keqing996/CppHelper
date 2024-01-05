#pragma once

#ifndef NOMINMAX
#   define NOMINMAX
#endif

#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif

#ifndef UNICODE
#   define UNICODE
#endif

#ifndef _UNICODE
#   define _UNICODE
#endif

#include <Windows.h>

namespace Helper::Win
{
    template<typename T>
    struct Coord
    {
        T x;
        T y;
    };

    template<typename T>
    struct Rect
    {
        T left;
        T top;
        T right;
        T bottom;
    };
}