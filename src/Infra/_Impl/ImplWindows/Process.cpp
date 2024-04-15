#include "../../Process.h"
#include "../../PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "../../Windows/WindowsDefine.h"
#include "../../String.h"
#include "ProcessHandle.h"
#include "ThreadHandle.h"
#include <Psapi.h>

#undef CreateProcess

namespace Infra
{
    int32_t Process::GetCurrentProcessId()
    {
        return ::GetCurrentProcessId();
    }

    ResPtr<ProcessHandle> Process::GetProcessHandle(int32_t processId)
    {
        ResPtr<ProcessHandle> pHandle(new ProcessHandle());
        pHandle->handle = ::OpenProcess(PROCESS_ALL_ACCESS, TRUE, processId);
        return pHandle;
    }

    void Process::ReleaseProcessHandle(ResPtr<ProcessHandle>&& pProcess)
    {
        if (pProcess == nullptr)
            return;

        ::CloseHandle(pProcess->handle);
    }

    std::string Process::GetProcessName(const ResPtr<ProcessHandle>& hProcess)
    {
        wchar_t nameBuffer[256 + 1] = {};
        const DWORD length = ::GetProcessImageFileNameW(hProcess->handle, nameBuffer, 256);
        if (length == 0)
            return {};

        return String::WideStringToString(nameBuffer).value();
    }

    std::optional<int> Process::CreateProcess(const std::string& commandLine, bool detach)
    {
        STARTUPINFO si;
        ::ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);

        PROCESS_INFORMATION pi;
        ::ZeroMemory( &pi, sizeof(pi) );

        std::wstring commandLineW = String::StringToWideString(commandLine).value();

        // Start the child process.
        if(!::CreateProcessW(
                nullptr,                                    // No module name (use command line)
                const_cast<LPWSTR>(commandLineW.c_str()),   // Command line
                nullptr,                                    // Process handle not inheritable
                nullptr,                                    // Thread handle not inheritable
                FALSE,                                      // Set handle inheritance to FALSE
                0,                                          // No creation flags
                nullptr,                                    // Use parent's environment block
                nullptr,                                    // Use parent's starting directory
                &si,                                        // Pointer to STARTUPINFO structure
                &pi )                                       // Pointer to PROCESS_INFORMATION structure
                )
        {
            return std::nullopt;
        }

        if (!detach)
        {
            ::WaitForSingleObject(pi.hProcess, INFINITE);

            DWORD dwExitCode = 0;
            ::GetExitCodeProcess(pi.hProcess, &dwExitCode);

            ::CloseHandle(pi.hProcess);
            ::CloseHandle(pi.hThread);
            return 0;
        }
        else
        {
            ::CloseHandle(pi.hProcess);
            ::CloseHandle(pi.hThread);
            return 0;
        }
    }

    std::optional<int> Process::CreateProcess(const ProcessCreateInfo& processCreateInfo)
    {
        HANDLE hStdInPipeRead = nullptr;
        HANDLE hStdInPipeWrite = nullptr;
        HANDLE hStdOutPipeRead = nullptr;
        HANDLE hStdOutPipeWrite = nullptr;

        STARTUPINFO si;
        ZeroMemory(&si, sizeof(STARTUPINFO));

        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

        SECURITY_ATTRIBUTES sa;
        ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));

        // Create one-way pipe for child process STDOUT
        if (!::CreatePipe(&hStdOutPipeRead, &hStdOutPipeWrite, &sa, 0))
            return std::nullopt;

        // Ensure read handle to pipe for STDOUT is not inherited
        if (!::SetHandleInformation(hStdOutPipeRead, HANDLE_FLAG_INHERIT, 0))
            return std::nullopt;

        // Create one-way pipe for child process STDIN
        if (!::CreatePipe(&hStdInPipeRead, &hStdInPipeWrite, &sa, 0))
            return std::nullopt;

        // Ensure write handle to pipe for STDIN is not inherited
        if (!::SetHandleInformation(hStdInPipeWrite, HANDLE_FLAG_INHERIT, 0))
            return std::nullopt;

        si.cb = sizeof(STARTUPINFO);
        si.hStdError = hStdOutPipeWrite;
        si.hStdOutput = hStdOutPipeWrite;
        si.hStdInput = hStdInPipeRead;
        si.dwFlags |= STARTF_USESTDHANDLES;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = nullptr;
        sa.bInheritHandle = true;   // Pipe handles are inherited

        std::wstring commandLineW = String::StringToWideString(processCreateInfo.commandLine).value();

        // Start the child process.
        if(!::CreateProcessW(
                nullptr,                                    // No module name (use command line)
                const_cast<LPWSTR>(commandLineW.c_str()),   // Command line
                nullptr,                                    // Process handle not inheritable
                nullptr,                                    // Thread handle not inheritable
                FALSE,                                      // Set handle inheritance to FALSE
                0,                                          // No creation flags
                nullptr,                                    // Use parent's environment block
                nullptr,                                    // Use parent's starting directory
                &si,                                        // Pointer to STARTUPINFO structure
                &pi )                                       // Pointer to PROCESS_INFORMATION structure
                )
        {
            return std::nullopt;
        }

        // Before close child process's handles:
        // ╔══════════════════╗                ╔══════════════════╗
        // ║  Parent Process  ║                ║  Child Process   ║
        // ╠══════════════════╣                ╠══════════════════╣
        // ║                  ║                ║                  ║
        // ║  hStdInPipeWrite ╟───────────────>║  hStdInPipeRead  ║
        // ║                  ║                ║                  ║
        // ║  hStdOutPipeRead ║<───────────────╢ hStdOutPipeWrite ║
        // ║                  ║                ║                  ║
        // ╚══════════════════╝                ╚══════════════════╝

        ::CloseHandle(hStdInPipeRead);
        ::CloseHandle(hStdOutPipeWrite);

        // After close child process's handles:
        // ╔══════════════════╗                ╔══════════════════╗
        // ║  Parent Process  ║                ║  Child Process   ║
        // ╠══════════════════╣                ╠══════════════════╣
        // ║                  ║                ║                  ║
        // ║  hStdInPipeWrite ╟───────────────>║                  ║
        // ║                  ║                ║                  ║
        // ║  hStdOutPipeRead ║<───────────────╢                  ║
        // ║                  ║                ║                  ║
        // ╚══════════════════╝                ╚══════════════════╝

        // Set stdin to outside
        if (processCreateInfo.pGetChildProcessSendStdIn != nullptr)
        {
            std::function<bool(const char*, int)> fSendStdIn = [&hStdInPipeWrite](const char* pData, int size) -> int
            {
                if (pData == nullptr)
                    return -1;

                DWORD realWrite = 0;
                const bool success = ::WriteFile(hStdInPipeWrite, pData, size, &realWrite, nullptr);
                if (!success)
                    return -1;

                return realWrite;
            };

            processCreateInfo.pGetChildProcessSendStdIn(fSendStdIn);
        }

        // Keep fetching stdout
        if (processCreateInfo.pChildProcessStdOutReceived != nullptr
            || processCreateInfo.pChildProcessStdOutFinished != nullptr)
        {
            const auto pBuffer = new char[1024];

            while (true)
            {
                DWORD readBytes = 0;
                const bool state = ::ReadFile(hStdOutPipeRead, pBuffer, 1024, &readBytes, nullptr);
                if (!state || readBytes == 0)
                {
                    if (processCreateInfo.pChildProcessStdOutFinished != nullptr)
                        processCreateInfo.pChildProcessStdOutFinished();

                    break;
                }

                if (processCreateInfo.pChildProcessStdOutReceived != nullptr)
                    processCreateInfo.pChildProcessStdOutReceived(pBuffer, readBytes);
            }

            delete[] pBuffer;
        }

        // Close left handle
        ::CloseHandle(hStdInPipeWrite);
        ::CloseHandle(hStdOutPipeRead);

        // Wait until child process exits
        ::WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD dwExitCode = 0;
        ::GetExitCodeProcess(pi.hProcess, &dwExitCode);

        // Close process and thread handles
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);

        return dwExitCode;
    }
}

#endif