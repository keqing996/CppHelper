#pragma once

#include "Windows/SocketWinImpl.hpp"

namespace Infra::Socket
{
    // Init and clear
    inline auto IsInitialized() -> bool
    {
        return Impl::IsInitialized();
    }

    inline auto InitEnvironment() -> bool
    {
        return Impl::InitEnvironment();
    }
    
    inline auto DestroyEnvironment() -> void
    {
        Impl::DestroyEnvironment();
    }

    // Create and destroy
    inline auto Create(AddressFamily family, Protocol protocol) -> std::optional<SocketHandle>
    {
        return Impl::Create(family, protocol);
    }

    inline auto Destroy(const SocketHandle& socketHandle) -> void
    {
        Impl::Destroy(socketHandle);
    }

    // Client
    template<AddressFamily addrFamily>
    inline auto Connect(const SocketHandle& socketHandle, const EndPoint<addrFamily>& endpoint, int timeOutInMs = -1) -> bool
    {
        return Impl::Connect(socketHandle, endpoint, timeOutInMs);
    }

    // Server
    template<AddressFamily addrFamily>
    inline auto Bind(const SocketHandle& socketHandle, const EndPoint<addrFamily>& endpoint) -> bool
    {
        return Impl::Bind(socketHandle, endpoint);
    }

    inline auto Listen(const SocketHandle& socketHandle) -> bool
    {
        return Impl::Listen(socketHandle);
    }

    inline auto Accept(const SocketHandle& socketHandle, int timeOutInMs = -1) -> std::optional<SocketHandle>
    {
        return Impl::Accept(socketHandle, timeOutInMs);
    }

    // Send & Recv
    inline auto Send(const SocketHandle& socketHandle, const char* pDataBuffer, int bufferSize) -> bool
    {
        return Impl::Send(socketHandle, pDataBuffer, bufferSize);
    }

    inline auto Receive(const SocketHandle& socketHandle, char* pDataBuffer, int bufferSize, int timeOutInMs = -1) -> std::optional<int>
    {
        return Impl::Receive(socketHandle, pDataBuffer, bufferSize, timeOutInMs);
    }
    
}