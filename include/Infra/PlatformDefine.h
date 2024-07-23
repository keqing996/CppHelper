#pragma once

/* Platform define */
#ifdef _MSC_VER
#   define PLATFORM_WINDOWS 1
#else
#   define PLATFORM_WINDOWS 0
#endif

#if defined(__clang__)
#    define COMPILER_CLANG 1
#    define COMPILER_MSVC 0
#    define COMPILER_GCC 0
#else
#    if defined(_MSC_VER)
#        define COMPILER_CLANG 0
#        define COMPILER_MSVC 1
#        define COMPILER_GCC 0
#    elif defined(__GNUC__)
#        define COMPILER_CLANG 0
#        define COMPILER_MSVC 0
#        define COMPILER_GCC 1
#    else
#        define COMPILER_CLANG 0
#        define COMPILER_MSVC 0
#        define COMPILER_GCC 0
#    endif
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

/* Cpp version
 * - std::format need gcc13
 */
#if defined(_MSC_VER)
#    if _MSVC_LANG >= 202002L
#        define CPP_VERSION 20L
#    elif _MSVC_LANG >= 201703L
#        define CPP_VERSION 17L
#    elif _MSVC_LANG >= 201402L
#        define CPP_VERSION 14L
#    else
#        define CPP_VERSION 11L
#    endif
#elif defined(__GNUC__) || defined(__clang__)
#    if __cplusplus >= 202002L
#        define CPP_VERSION 20L
#    elif __cplusplus >= 201703L
#        define CPP_VERSION 17L
#    elif __cplusplus >= 201402L
#        define CPP_VERSION 14L
#    else
#        define CPP_VERSION 11L
#    endif
#endif


