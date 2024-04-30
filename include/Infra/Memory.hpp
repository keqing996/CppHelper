#pragma once

#include <cstddef>
#include "PlatformDefine.hpp"

#if PLATFORM_WINDOWS
#   include "Windows/WindowsDefine.hpp"
#endif

namespace Infra
{
    class Memory
    {
    public:
        Memory() = delete;

    public:
        static void* VirtualReserve(void* wantedAddr, size_t size);
        static bool VirtualCommit(void* addr, size_t size);
        static void VirtualRelease(void* addr);
        static size_t CurrentPageSize();
    };

#pragma region [Win impl]

#if PLATFORM_WINDOWS

    void* Memory::VirtualReserve(void* wantedAddr, size_t size)
    {
        void* result = ::VirtualAlloc(wantedAddr, size, MEM_RESERVE, PAGE_READWRITE);
        if (result != nullptr)
            return result;

        return ::VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_READWRITE);
    }

    bool Memory::VirtualCommit(void* addr, size_t size)
    {
        return addr == ::VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE);
    }

    void Memory::VirtualRelease(void* addr)
    {
        ::VirtualFree(addr, 0, MEM_RELEASE);
    }

    size_t Memory::CurrentPageSize()
    {
        ::SYSTEM_INFO sysInfo;
        ::GetSystemInfo(&sysInfo);
        return sysInfo.dwPageSize;
    }

#endif

#pragma endregion
}