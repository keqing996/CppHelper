#pragma once

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

/* Assert */
#ifdef _DEBUG
#   if PLATFORM_WINDOWS
#       define ASSERT_CHECK(expr)
#   else
#       define ASSERT_CHECK(expr)
#   endif
#else
#   define ASSERT_CHECK(expr) (void(0))
#endif

