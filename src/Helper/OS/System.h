#pragma once

#include <string>
#include <memory>
#include <functional>
#include <optional>

#include "../PlatformDefine.h"

namespace Helper::OS
{
    struct ProcessHandle;
    struct ThreadHandle;

    class System
    {
    public:
        System() = delete;

    public:
        static auto GetMachineName() -> std::string;
        static auto GetCurrentUserName() -> std::string;

    public:
        using FChildProcessStdOutReceived = void (*) (const char*, int);
        using FChildProcessStdOutFinished = void (*) ();
        using FGetChildProcessSendStdIn = void (*) (const std::function<int(const char*, int)>&);

        struct ProcessCreateInfo
        {
            std::string commandLine;
            FChildProcessStdOutReceived pChildProcessStdOutReceived;
            FChildProcessStdOutFinished pChildProcessStdOutFinished;
            FGetChildProcessSendStdIn pGetChildProcessSendStdIn;
        };

        static auto GetCurrentProcessId() -> int32_t;
        static auto GetProcessHandle(int32_t processId) -> std::shared_ptr<ProcessHandle>;
        static auto ReleaseProcessHandle(const std::shared_ptr<ProcessHandle>& hProcess) -> void;
        static auto GetProcessName(const std::shared_ptr<ProcessHandle>& hProcess) -> std::string;
        static auto CreateProcess(const std::string& commandLine, bool detach = false) -> std::optional<int>;
        static auto CreateProcess(const ProcessCreateInfo& processCreateInfo) -> std::optional<int>;

    public:
        static auto GetEnviromentVariable(const std::string& keyName) -> std::string;
        static auto SetEnviromentVariable(const std::string& keyName, const std::string& value) -> void;

    public:
        static auto GetHomeDirectory() -> std::string;
        static auto GetCurrentDirectory() -> std::string;
        static auto SetCurrentDirectory(const std::string& path) -> bool;
        static auto GetExecutableDirectory() -> std::string;
        static auto GetTempDirectory() -> std::string;
    };
}