#pragma once

#include <string>
#include <memory>
#include <functional>
#include <optional>

namespace Helper
{
    struct ProcessHandle;
    struct ThreadHandle;

    class Process
    {
    public:
        Process() = delete;

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
        static auto GetProcessHandle(int32_t processId) -> std::unique_ptr<ProcessHandle>;
        static auto ReleaseProcessHandle(const std::unique_ptr<ProcessHandle>& hProcess) -> void;
        static auto GetProcessName(const std::unique_ptr<ProcessHandle>& hProcess) -> std::string;
        static auto CreateProcess(const std::string& commandLine, bool detach = false) -> std::optional<int>;
        static auto CreateProcess(const ProcessCreateInfo& processCreateInfo) -> std::optional<int>;

    };
}