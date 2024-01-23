#pragma once

#include <string>
#include <optional>

namespace Helper::Win::System
{
    namespace Impl
    {
        std::wstring Windows_GetUserName();
        std::optional<std::wstring> Windows_GetEnvVariable(const std::wstring& key);
    }

    struct ProcessResult
    {
        std::wstring stdOut;
        std::wstring stdErr;
    };

    inline std::wstring GetUserName()
    {
        return Impl::Windows_GetUserName();
    }

    inline std::optional<std::wstring> GetEnvironmentVariable(const std::wstring& key)
    {
        return Impl::Windows_GetEnvVariable(key);
    }

    inline bool CreateProcess(const std::wstring& cmdLine)
    {

    }

    void DoShellExecute(const std::wstring& app, const std::wstring& appPath, const std::wstring& appPara);

    void DoShellExecute(const std::wstring& app, const std::wstring& appPara);

}