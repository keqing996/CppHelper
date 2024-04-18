#include "../../String.h"
#include "../../PlatformDefine.h"

#if PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace Infra
{
    std::optional<std::string> String::WideStringToString(const std::wstring& wStr)
    {
        const wchar_t* wideStr = wStr.c_str();
        if (wideStr == nullptr)
            return std::string{};

#if !PLATFORM_WINDOWS
        int requiredSize = std::wcstombs( nullptr, wideStr, 0);

            std::vector<char> charBuffer(requiredSize + 1, 0);
            int size = std::wcstombs(charBuffer.data(), wideStr, requiredSize + 1);
            if (size < 0)
                return std::nullopt;

            return std::string(charBuffer.data());
#else
        int requiredSize = ::WideCharToMultiByte(CP_ACP,0,wideStr,
                                                 wStr.size(),nullptr,0,
                                                 nullptr,nullptr);

        std::vector<char> charBuffer(requiredSize + 1, 0);
        int size = ::WideCharToMultiByte(CP_ACP, 0, wideStr,
                                         wStr.size(), charBuffer.data(), requiredSize,
                                         nullptr, nullptr);
        if (size < 0)
            return std::nullopt;

        return std::string(charBuffer.data());
#endif
    }

    std::optional<std::wstring> String::StringToWideString(const std::string& str)
    {
        const char* multiBytesStr = str.c_str();
        if (multiBytesStr == nullptr)
            return std::wstring{};

#if !PLATFORM_WINDOWS
        int requiredSize = std::mbstowcs( nullptr, multiBytesStr, 0);

            std::vector<wchar_t> wideCharBuffer(requiredSize + 1, 0);
            int size = std::mbstowcs(wideCharBuffer.data(), multiBytesStr, requiredSize + 1);
            if (size < 0)
                return std::nullopt;

            return std::wstring(wideCharBuffer.data());
#else
        int requiredSize = ::MultiByteToWideChar(CP_ACP, 0, multiBytesStr,
                                                 str.size(), nullptr, 0);

        std::vector<wchar_t> wideCharBuffer(requiredSize + 1, 0);
        int size = ::MultiByteToWideChar(CP_ACP, 0, multiBytesStr,
                                         str.size(), wideCharBuffer.data(), requiredSize);
        if (size < 0)
            return std::nullopt;

        return std::wstring(wideCharBuffer.data());
#endif
    }
}
