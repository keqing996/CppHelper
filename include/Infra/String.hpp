#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <ranges>
#include <algorithm>
#include <optional>
#include "PlatformDefine.hpp"

#if PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace Infra
{
    class String
    {
    public:
        String() = delete;

    public:
        /// \brief Convert wide string to utf8 char string. Should set local before calling this.
        static std::string WideStringToString(const std::wstring& wStr)
        {
            const wchar_t* wideStr = wStr.c_str();
            if (wideStr == nullptr)
                return std::string{};

#if !PLATFORM_WINDOWS
            int requiredSize = std::wcstombs( nullptr, wideStr, 0);

            std::vector<char> charBuffer(requiredSize + 1, 0);
            int size = std::wcstombs(charBuffer.data(), wideStr, requiredSize + 1);
            if (size < 0)
                return std::string{};

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
                return std::string{};

            return std::string(charBuffer.data());
#endif
        }

        /// \brief Convert utf8 char string to wide string. Should set local before calling this.
        static std::wstring StringToWideString(const std::string& str)
        {
            const char* multiBytesStr = str.c_str();
            if (multiBytesStr == nullptr)
                return std::wstring{};

#if !PLATFORM_WINDOWS
            int requiredSize = std::mbstowcs( nullptr, multiBytesStr, 0);

            std::vector<wchar_t> wideCharBuffer(requiredSize + 1, 0);
            int size = std::mbstowcs(wideCharBuffer.data(), multiBytesStr, requiredSize + 1);
            if (size < 0)
                return std::wstring{};

            return std::wstring(wideCharBuffer.data());
#else
            int requiredSize = ::MultiByteToWideChar(CP_ACP, 0, multiBytesStr,
                                                 str.size(), nullptr, 0);

            std::vector<wchar_t> wideCharBuffer(requiredSize + 1, 0);
            int size = ::MultiByteToWideChar(CP_ACP, 0, multiBytesStr,
                                         str.size(), wideCharBuffer.data(), requiredSize);
            if (size < 0)
                return std::wstring{};

            return std::wstring(wideCharBuffer.data());
#endif
        }

        template <typename Encoding, typename DelimType>
        static std::vector<std::basic_string_view<Encoding>> SplitView(const std::basic_string<Encoding>& inputStr, DelimType delim)
        {
            auto split = std::views::split(inputStr, delim);

            std::vector<std::basic_string_view<Encoding>> result;
            for (const auto& element : split)
                result.emplace_back(element.begin(), element.end());

            return result;
        }

        template <typename Encoding, typename DelimType>
        static std::vector<std::basic_string<Encoding>> Split(const std::basic_string<Encoding>& inputStr, DelimType delim)
        {
            auto split = std::views::split(inputStr, delim);

            std::vector<std::basic_string<Encoding>> result;
            for (const auto& element : split)
                result.emplace_back(element.begin(), element.end());

            return result;
        }

        template <typename Encoding>
        static std::basic_string<Encoding> Join(const std::vector<std::basic_string<Encoding>>& strVec, const std::basic_string<Encoding>& delim)
        {
            std::basic_ostringstream<Encoding> oss;

            for (auto itr = strVec.begin(); itr != strVec.end(); ++itr)
            {
                oss << *itr;
                if (itr != strVec.end() - 1)
                    oss << delim;
            }

            return oss.str();
        }

        template <typename Encoding>
        static void Replace(std::basic_string<Encoding>& inStr, const std::basic_string<Encoding>& from, const std::basic_string<Encoding>& to)
        {
            size_t startPos = 0;
            while ((startPos = inStr.find(from, startPos)) != std::string::npos)
            {
                inStr.replace(startPos, from.length(), to);
                startPos += to.length();
            }
        }

        template <typename Encoding>
        static void TrimStart(std::basic_string<Encoding>& str)
        {
            str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](Encoding ch)
            {
                return !std::isspace(static_cast<int>(ch));
            }));
        }

        template <typename Encoding>
        static void TrimEnd(std::basic_string<Encoding>& str)
        {
            str.erase(std::find_if(str.rbegin(), str.rend(), [](Encoding ch)
            {
                return !std::isspace(static_cast<int>(ch));
            }).base(), str.end());
        }

        template <typename Encoding>
        static void Trim(std::basic_string<Encoding>& str)
        {
            TrimStart(str);
            TrimEnd(str);
        }
    };
}
