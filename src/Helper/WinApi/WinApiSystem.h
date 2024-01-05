#pragma once

#include <string>

namespace Helper::Win::System
{
    std::wstring GetWindowsUserName();

    bool CreateIndependentProcess(const std::wstring& cmdLine);

    void DoShellExecute(const std::wstring& app, const std::wstring& appPath, const std::wstring& appPara);

    void DoShellExecute(const std::wstring& app, const std::wstring& appPara);

}