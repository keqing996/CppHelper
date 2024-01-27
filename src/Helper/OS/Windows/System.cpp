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

    auto System::CreateProcessDettached(const std::string& commandLine) -> bool
    {
        STARTUPINFO si;
        ::ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);

        PROCESS_INFORMATION pi;
        ::ZeroMemory( &pi, sizeof(pi) );

        std::wstring commandLineW = String::StringToWideString(commandLine);

        // Start the child process.
        if( !::CreateProcessW(
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
            return false;
        }

        // Close process and thread handles.
        ::CloseHandle( pi.hProcess );
        ::CloseHandle( pi.hThread );

        return true;
    }

    auto System::CreateProcess(const std::string& commandLine,
        OnChildProcessStdOut pOnChildProcessStdOut,
        GetChildProcessStdIn pGetChildProcessStdIn) -> std::pair<bool, int>
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

        bool havePipe = pOnChildProcessStdOut != nullptr || pGetChildProcessStdIn != nullptr;
        if (havePipe)
        {
            // Create one-way pipe for child process STDOUT
            if (!::CreatePipe(&hStdOutPipeRead, &hStdOutPipeWrite, &sa, 0))
                return { false, 0 };

            // Ensure read handle to pipe for STDOUT is not inherited
            if (!::SetHandleInformation(hStdOutPipeRead, HANDLE_FLAG_INHERIT, 0))
                return { false, 0 };

            // Create one-way pipe for child process STDIN
            if (!::CreatePipe(&hStdInPipeRead, &hStdInPipeWrite, &sa, 0))
                return { false, 0 };

            // Ensure write handle to pipe for STDIN is not inherited
            if (!::SetHandleInformation(hStdInPipeWrite, HANDLE_FLAG_INHERIT, 0))
                return { false, 0 };
        }

        si.cb = sizeof(STARTUPINFO);
        if (havePipe)
        {
            si.hStdError = hStdOutPipeWrite;
            si.hStdOutput = hStdOutPipeWrite;
            si.hStdInput = hStdInPipeRead;
            si.dwFlags |= STARTF_USESTDHANDLES;
            sa.nLength = sizeof(SECURITY_ATTRIBUTES);
            sa.lpSecurityDescriptor = nullptr;
            // Pipe handles are inherited
            sa.bInheritHandle = true;
        }

        std::wstring commandLineW = String::StringToWideString(commandLine);

        // Start the child process.
        if( !::CreateProcessW(
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
            return { false, 0 };
        }

        if (havePipe)
        {
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

            if (pOnChildProcessStdOut != nullptr)
            {
                char* pBuffer = new char[1024];
                while (true)
                {
                    DWORD readBytes = 0;
                    bool state = ::ReadFile(hStdOutPipeRead, pBuffer, 1024, &readBytes, nullptr);
                    if (!state || readBytes == 0)
                        break;

                    pOnChildProcessStdOut(pBuffer, readBytes);
                }
            }
        }

        // Wait until child process exits
        ::WaitForSingleObject(pi.hProcess, INFINITE);

        // Close process and thread handles
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);

        DWORD dwExitCode = 0;
        ::GetExitCodeProcess(pi.hProcess, &dwExitCode);

        return { true, dwExitCode };
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

