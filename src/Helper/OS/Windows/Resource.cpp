#include "Resource.h"
#include "WindowsDefine.h"

#if PLATFORM_WINDOWS

#include <vector>

namespace Helper::OS
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

    template<>
    std::unique_ptr<DataResource> Resource::LoadResource(int id)
    {
        HINSTANCE hInst = GetModuleHandleW(nullptr);

        HRSRC findRes = ::FindResourceW(hInst, MAKEINTRESOURCE(id), RT_RCDATA);
        if (FAILED(findRes))
            return nullptr;

        const HGLOBAL mem = ::LoadResource(hInst, findRes);
        if (FAILED(mem))
            return nullptr;

        const auto pData = LockResource(mem);
        if (!pData)
            return nullptr;

        std::unique_ptr<DataResource> result(new DataResource());
        result->pData = pData;
        result->size = SizeofResource(hInst, findRes);

        return result;
    }

    template<>
    std::unique_ptr<IconResource> Resource::LoadResource(int id)
    {
        const auto hIcon = ::LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(id));
        if (hIcon == nullptr)
            return nullptr;

        std::unique_ptr<IconResource> result(new IconResource());
        result->hIcon = hIcon;
        return result;
    }

    template<>
    std::unique_ptr<BitmapResource> Resource::LoadResource(int id)
    {
        const auto hBitmap = ::LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(id));
        if (hBitmap == nullptr)
            return nullptr;

        std::unique_ptr<BitmapResource> result(new BitmapResource());
        result->hBitmap = hBitmap;
        return result;
    }

    template<>
    std::unique_ptr<CursorResource> Resource::LoadResource(int id)
    {
        const auto hCursor = ::LoadCursorW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(id));
        if (hCursor == nullptr)
            return nullptr;

        std::unique_ptr<CursorResource> result(new CursorResource());
        result->hCursor = hCursor;
        return result;
    }
}

#endif