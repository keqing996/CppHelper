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
    inline auto Create(AddressFamily family, Protocol protocol) -> ResPtr<SocketHandle>
    {
        return Impl::Create(family, protocol);
    }

    inline auto Destroy(ResPtr<SocketHandle>&& pSocket) -> void
    {
        Impl::Destroy(std::move(pSocket));
    }

    // Client
    template<AddressFamily addrFamily>
    inline auto Connect(const ResPtr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint, int timeOutInMs = -1) -> bool
    {
        return Impl::Connect(pSocket, endpoint, timeOutInMs);
    }

    // Server
    template<AddressFamily addrFamily>
    inline auto Bind(const ResPtr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint) -> bool
    {
        return Impl::Bind(pSocket, endpoint);
    }

    inline auto Listen(const ResPtr<SocketHandle>& pSocket) -> bool
    {
        return Impl::Listen(pSocket);
    }

    inline auto Accept(const ResPtr<SocketHandle>& pSocket, int timeOutInMs = -1) -> std::optional<ResPtr<SocketHandle>>
    {
        return Impl::Accept(pSocket, timeOutInMs);
    }

    // Send & Recv
    inline auto Send(const ResPtr<SocketHandle>& pSocket, const char* pDataBuffer, int bufferSize) -> bool
    {
        return Impl::Send(pSocket, pDataBuffer, bufferSize);
    }

    inline auto Receive(const ResPtr<SocketHandle>& pSocket, char* pDataBuffer, int bufferSize, int timeOutInMs = -1) -> std::optional<int>
    {
        return Impl::Receive(pSocket, pDataBuffer, bufferSize, timeOutInMs);
    }
    
}