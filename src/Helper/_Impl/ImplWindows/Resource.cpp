#include "../../Windows/Resource.h"
#include "../../Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include <vector>

namespace Helper
{
    template<>
    std::unique_ptr<DataResource> Resource::LoadResource(int id)
    {
        const HINSTANCE hInst = GetModuleHandleW(nullptr);

        const HRSRC findRes = ::FindResourceW(hInst, MAKEINTRESOURCE(id), RT_RCDATA);
        if (findRes != nullptr)
            return nullptr;

        const HGLOBAL mem = ::LoadResource(hInst, findRes);
        if (mem != nullptr)
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