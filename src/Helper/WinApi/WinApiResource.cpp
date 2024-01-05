
#include "WindowsPlatform.h"
#include "WinApiResource.h"

namespace Helper::Win::Resource
{
    template<>
    std::optional<DataResource> LoadResource(int id)
    {
        HINSTANCE hInst = GetModuleHandleW(nullptr);

        HRSRC findRes = ::FindResourceW(hInst, MAKEINTRESOURCE(id), RT_RCDATA);
        if (findRes)
        {
            HGLOBAL mem = ::LoadResource(hInst, findRes);
            if (mem)
            {
                DWORD size = SizeofResource(hInst, findRes);
                void* data = LockResource(mem);

                if (data)
                {
                    DataResource result { data, static_cast<unsigned int>(size) };
                    return result;
                }
                else
                    return std::nullopt;
            }
            else
                return std::nullopt;
        }
        else
            return std::nullopt;
    }

    template<>
    std::optional<IconResource> LoadResource(int id)
    {
        auto hIcon = ::LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(id));
        if (hIcon == nullptr)
            return std::nullopt;

        IconResource result { hIcon };
        return result;
    }

    template<>
    std::optional<BitmapResource> LoadResource(int id)
    {
        auto hBitmap = ::LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(id));
        if (hBitmap == nullptr)
            return std::nullopt;

        BitmapResource result { hBitmap };
        return result;
    }

    template<>
    std::optional<CursorResource> LoadResource(int id)
    {
        auto hCursor = ::LoadCursorW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(id));
        if (hCursor == nullptr)
            return std::nullopt;

        CursorResource result { hCursor };
        return result;
    }
}