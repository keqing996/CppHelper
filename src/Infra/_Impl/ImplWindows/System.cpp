#include "../../System.h"
#include "../../Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include "../../String.h"
#include <vector>
#include <Psapi.h>

#undef GetEnviromentVariable
#undef SetEnviromentVariable
#undef GetCurrentDirectory
#undef SetCurrentDirectory

namespace Infra
{
    std::string System::GetMachineName()
    {
        DWORD bufferSize = MAX_COMPUTERNAME_LENGTH + 1;
        wchar_t nameBuffer[bufferSize];
        if (!::GetComputerNameW(nameBuffer, &bufferSize))
            return {};

        return String::WideStringToString(nameBuffer).value();
    }

    std::string System::GetCurrentUserName()
    {
        DWORD bufferSize = 256 + 1;
        wchar_t nameBuffer[bufferSize];
        if (!::GetUserNameW(nameBuffer, &bufferSize))
            return {};

        return String::WideStringToString(nameBuffer).value();
    }

    std::string System::GetEnviromentVariable(const std::string& keyName)
    {
        std::vector<wchar_t> buffer(1024, 0);
        const std::wstring keyNameW = String::StringToWideString(keyName).value();

        DWORD ret = ::GetEnvironmentVariableW(keyNameW.c_str(), buffer.data(), 1024 - 1);
        if (ret == 0)
            return {};

        if (ret < 1024 - 1)
            return String::WideStringToString(buffer.data()).value();

        buffer.resize(ret + 1);
        ret = ::GetEnvironmentVariableW(keyNameW.c_str(), buffer.data(), ret);

        return String::WideStringToString(buffer.data()).value();
    }

    void System::SetEnviromentVariable(const std::string& keyName, const std::string& value)
    {
        const std::wstring keyNameW = String::StringToWideString(keyName).value();

        if (value.empty())
        {
            ::SetEnvironmentVariableW(keyNameW.c_str(), nullptr);
        }
        else
        {
            const std::wstring valueW = String::StringToWideString(value).value();
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
        return String::WideStringToString(result).value();
    }

    bool System::SetCurrentDirectory(const std::string& path)
    {
        std::wstring pathW = String::StringToWideString(path).value();
        return ::SetCurrentDirectoryW(pathW.c_str());
    }

    std::string System::GetExecutableDirectory()
    {
        wchar_t buffer[MAX_PATH + 1] = {};
        ::GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        return String::WideStringToString(buffer).value();
    }

    std::string System::GetTempDirectory()
    {
        wchar_t buffer[MAX_PATH + 1] = {};
        ::GetTempPathW(MAX_PATH, buffer);
        return String::WideStringToString(buffer).value();
    }
}

#endif

