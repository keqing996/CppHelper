#include "WinApiSystem.h"
#include "WindowsPlatform.h"
#include <vector>
#include <shellapi.h>

namespace Helper::Win::System::Impl
{
    std::wstring Windows_GetUserName()
    {
        wchar_t userNameBuffer[255];
        DWORD userNameBufferLen;
        ::GetUserName(userNameBuffer, &userNameBufferLen);
        return { userNameBuffer };
    }

    std::optional<std::wstring> Windows_GetEnvVariable(const std::wstring& key)
    {
        DWORD bufferSize = GetEnvironmentVariable(key.c_str(), nullptr, 0);
        if (bufferSize <= 0)
            return std::nullopt;

        std::vector<wchar_t> result(bufferSize);
        GetEnvironmentVariable(key.c_str(), result.data(), bufferSize);

        return { result.data() };
    }

    bool CreateIndependentProcess(const std::wstring& cmdLine)
    {
        STARTUPINFO si;
        ::ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);

        PROCESS_INFORMATION pi;
        ::ZeroMemory( &pi, sizeof(pi) );

        // Start the child process.
        if( !::CreateProcessW(
                nullptr,                                // No module name (use command line)
                const_cast<LPWSTR>(cmdLine.c_str()),        // Command line
                nullptr,                                // Process handle not inheritable
                nullptr,                                // Thread handle not inheritable
                FALSE,                                  // Set handle inheritance to FALSE
                0,                                      // No creation flags
                nullptr,                                // Use parent's environment block
                nullptr,                                // Use parent's starting directory
                &si,                                    // Pointer to STARTUPINFO structure
                &pi )                                   // Pointer to PROCESS_INFORMATION structure
                )
        {
            return false;
        }

        // Wait until child process exits.
        ::WaitForSingleObject( pi.hProcess, INFINITE );

        // Close process and thread handles.
        ::CloseHandle( pi.hProcess );
        ::CloseHandle( pi.hThread );

        return true;
    }

    void DoShellExecute(const std::wstring& app, const std::wstring& appPath, const std::wstring& appPara)
    {
        ::ShellExecuteW(nullptr,
                        nullptr,
                        app.c_str(),
                        appPara.c_str(),
                        appPath.c_str(),
                        SW_SHOWNORMAL);
    }

    void DoShellExecute(const std::wstring& app, const std::wstring& appPara)
    {
        ::ShellExecuteW(nullptr,
                        nullptr,
                        app.c_str(),
                        appPara.c_str(),
                        nullptr,
                        SW_SHOWNORMAL);
    }
}
