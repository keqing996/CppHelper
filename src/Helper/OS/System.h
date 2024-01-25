#pragma once

#include <string>
#include <memory>
#include <functional>
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
        using OnChildProcessStdOut = void(*)(const char*, int);
        using SetChildProcessStdIn = bool(*)(const char*, int);
        using GetChildProcessStdIn = SetChildProcessStdIn(*)();

        static auto GetCurrentProcessId() -> int32_t;
        static auto GetProcessHandle(int32_t processId) -> std::shared_ptr<ProcessHandle>;
        static auto ReleaseProcessHandle(const std::shared_ptr<ProcessHandle>& hProcess) -> void;
        static auto GetProcessName(const std::shared_ptr<ProcessHandle>& hProcess) -> std::string;
        static auto CreateProcessDettached(const std::string& commandLine) -> bool;
        static auto CreateProcess(const std::string& commandLine, OnChildProcessStdOut pOnChildProcessStdOut = nullptr,
            GetChildProcessStdIn pGetChildProcessStdIn = nullptr) -> std::pair<bool, int>;

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