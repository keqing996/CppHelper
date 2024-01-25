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

    ProcessHandle System::GetProcessHandle(int32_t processId)
    {
        const HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, TRUE, processId);
        return { hProcess };
    }

    void System::ReleaseProcessHandle(ProcessHandle hProcess)
    {
        ::CloseHandle(hProcess.handle);
    }

    std::string System::GetProcessName(ProcessHandle hProcess)
    {
        wchar_t nameBuffer[256 + 1] = {};
        const DWORD length = ::GetProcessImageFileNameW(hProcess.handle, nameBuffer, 256);
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
}

#endif

