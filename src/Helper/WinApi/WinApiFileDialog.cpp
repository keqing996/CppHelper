
#include "WindowsPlatform.h"
#include "WinApiFileDialog.h"

#include <shobjidl.h>

namespace Helper::Win::FileDialog
{
    std::optional<std::wstring> ShowShellDialogAndGetResult(IFileDialog* pFileDialog)
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
                PWSTR filePath;
                hr = pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);

                if (SUCCEEDED(hr))
                {
                    result = filePath;
                    getResultSuccess = true;
                    CoTaskMemFree(filePath);
                }

                pShellItem->Release();
            }
        }

        if (getResultSuccess)
            return result;

        return std::nullopt;
    }

    std::optional<std::wstring> OpenFile(const std::wstring& titleMsg, const std::vector<FileTypeFilter>* pFilter)
    {
        std::optional<std::wstring> result;
        IFileDialog* pFileDialog;
        COMDLG_FILTERSPEC* pFileFilterArray = nullptr;

        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (!SUCCEEDED(hr))
            return std::nullopt;

        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                              IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileDialog));

        if (!SUCCEEDED(hr))
            return std::nullopt;

        // title
        pFileDialog->SetTitle(titleMsg.c_str());

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

        // show
        result = ShowShellDialogAndGetResult(pFileDialog);

        // clear up
        pFileDialog->Release();
        CoUninitialize();

        delete[] pFileFilterArray;

        return result;
    }

    std::optional<std::wstring> SaveFile(const std::wstring& titleMsg, const std::wstring& defaultName, const std::vector<FileTypeFilter>* pFilter)
    {
        std::optional<std::wstring> result;
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
        pFileDialog->SetTitle(titleMsg.c_str());

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
        pFileDialog->SetFileName(defaultName.c_str());

        // show
        result = ShowShellDialogAndGetResult(pFileDialog);

        // clear up
        pFileDialog->Release();
        CoUninitialize();

        delete[] pFileFilterArray;

        return result;
    }

    std::optional<std::wstring> OpenDirectory(const std::wstring& titleMsg)
    {
        std::optional<std::wstring> result;
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
        pFileDialog->SetTitle(titleMsg.c_str());

        // show
        result = ShowShellDialogAndGetResult(pFileDialog);

        // clear up
        pFileDialog->Release();
        CoUninitialize();

        return result;
    }
}