#pragma once

#include <string>
#include <memory>
#include <functional>
#include <optional>

#include "ResourcePtr.h"

namespace Infra
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
        static auto GetProcessHandle(int32_t processId) -> ResPtr<ProcessHandle>;
        static auto ReleaseProcessHandle(ResPtr<ProcessHandle>&& pProcess) -> void;
        static auto GetProcessName(const ResPtr<ProcessHandle>& hProcess) -> std::string;
        static auto CreateProcess(const std::string& commandLine, bool detach = false) -> std::optional<int>;
        static auto CreateProcess(const ProcessCreateInfo& processCreateInfo) -> std::optional<int>;

    };
}