
#pragma once

#include <optional>
#include <concepts>

namespace Helper::Win::Resource
{
    struct WinRcResource {};

    struct DataResource: public WinRcResource
    {
        void* data;
        unsigned int size;

        DataResource(void* d, unsigned int s)
            : data(d)
            , size(s)
        {
        }
    };

    struct IconResource: public WinRcResource
    {
        void* hIcon;

        explicit IconResource(void* handle)
            : hIcon(handle)
        {
        }
    };

    struct BitmapResource: public WinRcResource
    {
        void* hBitmap;

        explicit BitmapResource(void* handle)
                : hBitmap(handle)
        {
        }
    };

    struct CursorResource: public WinRcResource
    {
        void* hCursor;

        explicit CursorResource(void* handle)
                : hCursor(handle)
        {
        }
    };

    template<typename T>
    concept ConceptRcResource = std::derived_from<T, WinRcResource>;

    template<typename T> requires ConceptRcResource<T>
    std::optional<T> LoadResource(int id);
}