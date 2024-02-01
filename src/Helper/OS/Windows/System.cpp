#include "../System.h"
#include "WindowsDefine.h"

#if PLATFORM_WINDOWS

#include "../../String.h"
#include <vector>
#include <Psapi.h>

#undef GetEnviromentVariable
#undef SetEnviromentVariable
#undef GetCurrentDirectory
#undef SetCurrentDirectory
#undef CreateProcess

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

    std::unique_ptr<ProcessHandle> System::GetProcessHandle(int32_t processId)
    {
        std::unique_ptr<ProcessHandle> pHandle(new ProcessHandle());
        pHandle->handle = ::OpenProcess(PROCESS_ALL_ACCESS, TRUE, processId);
        return pHandle;
    }

    void System::ReleaseProcessHandle(const std::unique_ptr<ProcessHandle>& hProcess)
    {
        ::CloseHandle(hProcess->handle);
    }

    std::string System::GetProcessName(const std::unique_ptr<ProcessHandle>& hProcess)
    {
        wchar_t nameBuffer[256 + 1] = {};
        const DWORD length = ::GetProcessImageFileNameW(hProcess->handle, nameBuffer, 256);
        if (length == 0)
            return {};

        return String::WideStringToString(nameBuffer);
    }

    std::optional<int> CreateProcess(const std::string& commandLine, bool detach)
    {
        STARTUPINFO si;
        ::ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);

        PROCESS_INFORMATION pi;
        ::ZeroMemory( &pi, sizeof(pi) );

        std::wstring commandLineW = String::StringToWideString(commandLine);

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

    std::optional<int> System::CreateProcess(const ProcessCreateInfo& processCreateInfo)
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

        std::wstring commandLineW = String::StringToWideString(processCreateInfo.commandLine);

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

        // Before close child prcess's handles:
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

