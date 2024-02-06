#pragma once

#include "../PlatformDefine.h"
#include "../ResourcePtr.h"

#if PLATFORM_WINDOWS

#include <memory>

namespace Helper
{
    struct DataResource;
    struct CursorResource;
    struct BitmapResource;
    struct IconResource;

    class Resource
    {
    public:
        Resource() = delete;

    public:
        template<typename T>
        std::unique_ptr<T> LoadResource(int id);
    };
}

#endif