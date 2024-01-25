#pragma once

#include <string>
#include "../PlatformDefine.h"

namespace Helper::OS
{
    struct ProcessHandle;

    class System
    {
    public:
        System() = delete;

    public:
        static std::string GetMachineName();
        static std::string GetCurrentUserName();

    public:
        static int32_t GetCurrentProcessId();
        static ProcessHandle GetProcessHandle(int32_t processId);
        static void ReleaseProcessHandle(ProcessHandle hProcess);
        static std::string GetProcessName(ProcessHandle hProcess);

    public:
        static std::string GetEnviromentVariable(const std::string& keyName);
        static void SetEnviromentVariable(const std::string& keyName, const std::string& value);
        static std::string GetHomeDirectory();
    };
}