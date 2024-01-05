#pragma once

#include <string>
#include <vector>
#include <optional>

namespace Helper::Win::FileDialog
{
    /*
     * example { { L"Text Files", L"*.txt" }, { L"All Files", L"*.*" } }
     */
    struct FileTypeFilter
    {
        std::wstring name;
        std::wstring suffix;
    };

    std::optional<std::wstring> OpenFile(const std::wstring& titleMsg,
                                         const std::vector<FileTypeFilter>* pFilter = nullptr);

    std::optional<std::wstring> SaveFile(const std::wstring& titleMsg,
                                         const std::wstring& defaultName,
                                         const std::vector<FileTypeFilter>* pFilter = nullptr);

    std::optional<std::wstring> OpenDirectory(const std::wstring& titleMsg);
}