
#include "WinApiMemory.h"
#include "WindowsPlatform.h"

namespace Helper::Win::Memory
{
    void* VirtualMemoryReserve(void* wantedAddress, size_t size)
    {
        void* result = ::VirtualAlloc(wantedAddress, size, MEM_RESERVE, PAGE_READWRITE);
        if (result != nullptr)
            return result;

        return ::VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_READWRITE);
    }

    bool VirtualMemoryCommit(void* address, size_t size)
    {
        void* result = ::VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE);
        return result == address;
    }

    void* VirtualMemoryRelease(void* address, size_t size)
    {
        ::VirtualFree(address, size, MEM_RELEASE);
    }
}