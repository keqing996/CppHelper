#pragma once

#include <cassert>

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

/* Assert - use comma operator,
 * need extra parentheses so the comma isn't treated as a delimiter between the arguments.
 * example: assert(("A must be equal to B", a == b));
 */
#ifdef NDEBUG
#   define ASSERT_MSG(expr, msg)
#else
#   define ASSERT_MSG(expr, msg) assert(((void)(msg), (expr)))
#endif

/* Cpp version
 * - std::format need gcc13
 */
#ifdef _MSVC_LANG
#    if _MSVC_LANG >= 202002L
#        define CPP_VERSION 20L
#    elif _MSVC_LANG >= 201703L
#        define CPP_VERSION 17L
#    elif _MSVC_LANG >= 201402L
#        define CPP_VERSION 14L
#    else
#        define CPP_VERSION 11L
#    endif
#endif

#ifdef __cplusplus
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


