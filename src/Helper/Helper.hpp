#include <vector>
#include <string>
#include <functional>
#include <filesystem>

/* Platform define */
#ifdef _MSC_VER
#   define PLATFORM_WINDOWS 1
#else
#   define PLATFORM_WINDOWS 0
#endif

#ifdef __APPLE__
#   ifdef TARGET_OS_IPHONE
#       define PLATFORM_IOS 1
#   else
#       define PLATFORM_IOS 0
#   endif
#   ifdef TARGET_OS_MAC
#       define PLATFORM_MAC 1
#   else
#       define PLATFORM_MAC 0
#   endif
#else
#   define PLATFORM_IOS 0
#   define PLATFORM_MAC 0
#endif

#ifdef __ANDROID__
#   define PLATFORM_ANDROID 1
#else
#   define PLATFORM_ANDROID 0
#endif

#ifdef __linux__
#   define PLATFORM_LINUX 1
#else
#   define PLATFORM_LINUX 0
#endif

/* Posix support */
#define PLATFORM_SUPPORT_POSIX (PLATFORM_LINUX || PLATFORM_ANDROID || PLATFORM_MAC || PLATFORM_IOS)

#if PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace Helper
{
    class String
    {
    public:
        String() = delete;

    public:
        static std::vector<std::string> Split(const std::string& sourceStr, const std::string& delimiter)
        {
            size_t posStart = 0;
            size_t posEnd;
            size_t delimLength = delimiter.length();
            std::string token;
            std::vector<std::string> res;

            while ((posEnd = sourceStr.find(delimiter, posStart)) != std::string::npos)
            {
                token = sourceStr.substr(posStart, posEnd - posStart);
                posStart = posEnd + delimLength;
                res.push_back(token);
            }

            res.push_back(sourceStr.substr(posStart));
            return res;
        }

        static std::vector<std::string> SplitCsvLine(const std::string& sourceStr)
        {
            auto subStringStripDoubleQuote = [&](size_t posStart, size_t posEnd) -> std::string
            {
                if (sourceStr[posStart] == '"' && sourceStr[posEnd - 1] == '"' && posEnd - posStart > 2)
                {
                    posStart++;
                    posEnd--;
                }

                return sourceStr.substr(posStart, posEnd - posStart);
            };

            std::vector<std::string> res;
            bool currentInDoubleQuote = false;
            size_t posStart = 0;
            size_t posEnd = posStart;

            while (true)
            {
                if (posEnd == sourceStr.size())
                    break;

                if (sourceStr[posEnd] == '"')
                    currentInDoubleQuote = !currentInDoubleQuote;

                if (!currentInDoubleQuote && sourceStr[posEnd] == ',')
                {
                    res.push_back(subStringStripDoubleQuote(posStart, posEnd));
                    posStart = posEnd + 1;
                }

                posEnd++;
            }

            res.push_back(subStringStripDoubleQuote(posStart, posEnd));

            return res;
        }

        static std::optional<std::string> String::WideStringToString(const std::wstring& wStr)
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

        static std::optional<std::wstring> String::StringToWideString(const std::string& str)
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
    };

    class NonCopyable
    {
    public:
        NonCopyable() = default;
        ~NonCopyable() = default;

    public:
        NonCopyable( const NonCopyable& ) = delete;
        const NonCopyable& operator=( const NonCopyable& ) = delete;
    };

    class ScopeGuard : public NonCopyable
    {
    public:
        ScopeGuard(ScopeGuard&& other) = delete;

        ~ScopeGuard()
        {
            _cleanup();
        }

        template<class Func>
        ScopeGuard(const Func& cleanup) // NOLINT(*-explicit-constructor)
                : _cleanup(cleanup)
        {
        }

    private:
        std::function<void()> _cleanup;
    };

    class System
    {
    public:
        System() = delete;

    public:
        static void EnsureDirectoryExist(const std::string& pathStr)
        {
            std::filesystem::path path(pathStr);
            std::filesystem::path directory = std::filesystem::is_directory(path) ? path : path.parent_path();

            if (std::filesystem::exists(directory))
                std::filesystem::create_directory(directory);
        }

        static std::string GetFileName(const std::string& filePath)
        {
            return std::filesystem::path(filePath).filename().string();
        }

        static std::string GetFileNameWithoutExtension(const std::string& filePath)
        {
            return std::filesystem::path(filePath).stem().string();
        }

        static std::string GetFileExtension(const std::string& filePath)
        {
            return std::filesystem::path(filePath).extension().string();
        }
    };
}