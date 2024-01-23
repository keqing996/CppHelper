#pragma once


namespace Helper::Win::Memory
{
    void* VirtualMemoryReserve(void* wantedAddress, size_t size);
    bool VirtualMemoryCommit(void* address, size_t size);
    void* VirtualMemoryRelease(void* address, size_t size);
}