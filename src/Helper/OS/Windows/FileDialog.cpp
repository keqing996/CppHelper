#include "../FileDialog.h"
#include "WindowsDefine.h"

#if PLATFORM_WINDOWS

#include <memory>
#include <shobjidl.h>
#include "../../String.h"
#include "../../ScopeGuard.h"

namespace Helper::OS
{
    struct FileTypeFilterW
    {
        std::wstring name;
        std::wstring suffix;
    };

    FileTypeFilterW GetWideStringVersion(const FileDialog::FileTypeFilter& fileFilter)
    {
        return FileTypeFilterW {
            String::StringToWideString(fileFilter.name),
            String::StringToWideString(fileFilter.suffix)
        };
    }

    std::optional<std::string> ShowShellDialogAndGetResult(IFileDialog* pFileDialog)
    {
        std::wstring result;
        bool getResultSuccess = false;

        // show
        HRESULT hr = pFileDialog->Show(nullptr);
        if (SUCCEEDED(hr))
        {
            IShellItem* pShellItem;
            hr = pFileDialog->GetResult(&pShellItem);
            if (SUCCEEDED(hr))
            {
                ScopeGuard guardShellItem = [&]{ pShellItem->Release(); };
                {
                    PWSTR filePath;
                    hr = pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);

                    if (SUCCEEDED(hr))
                    {
                        result = filePath;
                        getResultSuccess = true;
                        CoTaskMemFree(filePath);
                    }
                }
            }
        }

        if (getResultSuccess)
            return String::WideStringToString(result);

        return std::nullopt;
    }

    std::optional<std::string> FileDialog::OpenFile(const std::string& titleMsg, const std::vector<FileTypeFilter>* pFilter)
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (!SUCCEEDED(hr))
            return std::nullopt;

        IFileDialog* pFileDialogRaw;
        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                              IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileDialogRaw));

        if (!SUCCEEDED(hr))
            return std::nullopt;

        auto FileDialogDeleter = [](IFileDialog* p)
        {
            p->Release();
        };

        std::unique_ptr<IFileDialog, decltype(FileDialogDeleter)> pFileDialog(pFileDialogRaw, FileDialogDeleter);

        // title
        const auto titleMsgW = String::StringToWideString(titleMsg);
        hr = pFileDialog->SetTitle(titleMsgW.c_str());
        if (!SUCCEEDED(hr))
            return std::nullopt;

        // filter
        if (pFilter != nullptr)
        {
            std::vector<FileTypeFilterW> filterVec;
            for (const auto& filter : *pFilter)
                filterVec.push_back(GetWideStringVersion(filter));

            std::vector<COMDLG_FILTERSPEC> filterSpecVec;
            for (const auto& [filterName, filterSuffic] : filterVec)
            {
                COMDLG_FILTERSPEC spec;
                spec.pszName = filterName.c_str();
                spec.pszSpec = filterSuffic.c_str();
                filterSpecVec.push_back(spec);
            }

            hr = pFileDialog->SetFileTypes(pFilter->size() * sizeof(COMDLG_FILTERSPEC), filterSpecVec.data());
            if (!SUCCEEDED(hr))
                return std::nullopt;
        }

        // show
        std::optional<std::string> result = ShowShellDialogAndGetResult(pFileDialog);

        // clear up
        CoUninitialize();

        return result;
    }

    std::optional<std::string> FileDialog::SaveFile(const std::string& titleMsg, const std::string& defaultName, const std::vector<FileTypeFilter>* pFilter)
    {
        IFileDialog* pFileDialog;
        COMDLG_FILTERSPEC* pFileFilterArray = nullptr;

        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (!SUCCEEDED(hr))
            return std::nullopt;

        hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_ALL,
                              IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileDialog));

        if (!SUCCEEDED(hr))
            return std::nullopt;

        // title
        const auto titleMsgW = String::StringToWideString(titleMsg);
        hr = pFileDialog->SetTitle(titleMsgW.c_str());
        if (!SUCCEEDED(hr))
            return std::nullopt;

        // filter
        if (pFilter != nullptr)
        {
            pFileFilterArray = new COMDLG_FILTERSPEC[pFilter->size()];
            for (int i = 0; i < pFilter->size(); i++)
            {
                pFileFilterArray[i].pszName = (*pFilter)[i].name.c_str();
                pFileFilterArray[i].pszSpec = (*pFilter)[i].suffix.c_str();
            }
            pFileDialog->SetFileTypes(pFilter->size() * sizeof(COMDLG_FILTERSPEC), pFileFilterArray);
        }

        // save name
        pFileDialog->SetFileName(String::StringToWideString(defaultName).c_str());

        // show
        std::optional<std::string> result = ShowShellDialogAndGetResult(pFileDialog);

        // clear up
        pFileDialog->Release();
        CoUninitialize();

        delete[] pFileFilterArray;

        return result;
    }

    std::optional<std::string> FileDialog::OpenDirectory(const std::string& titleMsg)
    {
        IFileDialog* pFileDialog;

        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (!SUCCEEDED(hr))
            return std::nullopt;

        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                              IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileDialog));

        if (!SUCCEEDED(hr))
            return std::nullopt;

        // set directory
        DWORD options;
        hr = pFileDialog->GetOptions(&options);
        if (SUCCEEDED(hr))
            pFileDialog->SetOptions(options | FOS_PICKFOLDERS);

        // title
        const auto titleMsgW = String::StringToWideString(titleMsg);
        hr = pFileDialog->SetTitle(titleMsgW.c_str());
        if (!SUCCEEDED(hr))
            return std::nullopt;

        // show
        std::optional<std::string> result = ShowShellDialogAndGetResult(pFileDialog);

        // clear up
        pFileDialog->Release();
        CoUninitialize();

        return result;
    }

}

#endif