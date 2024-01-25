#include "../System.h"

#include "WindowsDefine.h"
#include "../../String.h"

#include <vector>
#include <Psapi.h>

#if PLATFORM_WINDOWS

#undef GetEnviromentVariable
#undef SetEnviromentVariable
#undef GetCurrentDirectory
#undef SetCurrentDirectory

namespace Helper::OS
{
    struct ProcessHandle
    {
        HANDLE handle;
    };

    struct ThreadHandle
    {
        HANDLE handle;
    };

    std::string System::GetMachineName()
    {
        DWORD bufferSize = MAX_COMPUTERNAME_LENGTH + 1;
        wchar_t nameBuffer[bufferSize] = {};
        if (!::GetComputerNameW(nameBuffer, &bufferSize))
            return {};

        return String::WideStringToString(nameBuffer);
    }

    std::string System::GetCurrentUserName()
    {
        DWORD bufferSize = 256 + 1;
        wchar_t nameBuffer[bufferSize] = {};
        if (!::GetUserNameW(nameBuffer, &bufferSize))
            return {};

        return String::WideStringToString(nameBuffer);
    }

    int32_t System::GetCurrentProcessId()
    {
        return ::GetCurrentProcessId();
    }

    std::shared_ptr<ProcessHandle> System::GetProcessHandle(int32_t processId)
    {
        auto pHandle = std::make_shared<ProcessHandle>();
        pHandle->handle = ::OpenProcess(PROCESS_ALL_ACCESS, TRUE, processId);
        return pHandle;
    }

    void System::ReleaseProcessHandle(const std::shared_ptr<ProcessHandle>& hProcess)
    {
        ::CloseHandle(hProcess->handle);
    }

    std::string System::GetProcessName(const std::shared_ptr<ProcessHandle>& hProcess)
    {
        wchar_t nameBuffer[256 + 1] = {};
        const DWORD length = ::GetProcessImageFileNameW(hProcess->handle, nameBuffer, 256);
        if (length == 0)
            return {};

        return String::WideStringToString(nameBuffer);
    }

    std::string System::GetEnviromentVariable(const std::string& keyName)
    {
        std::vector<wchar_t> buffer(1024, 0);
        const std::wstring keyNameW = String::StringToWideString(keyName);

        DWORD ret = ::GetEnvironmentVariableW(keyNameW.c_str(), buffer.data(), 1024 - 1);
        if (ret == 0)
            return {};

        if (ret < 1024 - 1)
            return String::WideStringToString(buffer.data());

        buffer.resize(ret + 1);
        ret = ::GetEnvironmentVariableW(keyNameW.c_str(), buffer.data(), ret);

        return String::WideStringToString(buffer.data());
    }

    void System::SetEnviromentVariable(const std::string& keyName, const std::string& value)
    {
        const std::wstring keyNameW = String::StringToWideString(keyName);

        if (value.empty())
        {
            ::SetEnvironmentVariableW(keyNameW.c_str(), nullptr);
        }
        else
        {
            const std::wstring valueW = String::StringToWideString(value);
            ::SetEnvironmentVariableW(keyNameW.c_str(), valueW.c_str());
        }
    }

    std::string System::GetHomeDirectory()
    {
        std::string result = GetEnviromentVariable("USERPROFILE");

        if (result.empty())
            result = GetEnviromentVariable("HOMEDRIVE") + GetEnviromentVariable("HOMEPATH");

        return result;
    }

    std::string System::GetCurrentDirectory()
    {
        std::wstring result;
        const auto size = ::GetCurrentDirectoryW(0, nullptr);
        result.resize(size);
        ::GetCurrentDirectoryW(size, result.data());
        return String::WideStringToString(result);
    }

    bool System::SetCurrentDirectory(const std::string& path)
    {
        std::wstring pathW = String::StringToWideString(path);
        return ::SetCurrentDirectoryW(pathW.c_str());
    }

    std::string System::GetExecutableDirectory()
    {
        wchar_t buffer[MAX_PATH + 1] = {};
        ::GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        return String::WideStringToString(buffer);
    }

    std::string System::GetTempDirectory()
    {
        wchar_t buffer[MAX_PATH + 1] = {};
        ::GetTempPathW(MAX_PATH, buffer);
        return String::WideStringToString(buffer);
    }
}

#endif

