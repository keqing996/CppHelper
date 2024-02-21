#pragma once

#include <memory>

#include "PlatformDefine.h"
#include "SocketGeneral.h"
#include "ResourcePtr.h"

namespace Infra::Socket
{
    struct SocketHandle;

    // Init and clean
    auto IsInitialized() -> bool;
    auto InitEnvironment() -> State;
    auto DestroyEnvironment() -> void;

    // Create and destroy
    auto Create(AddressFamily family, Protocol protocol) -> ResPtr<SocketHandle>;
    auto Destroy(ResPtr<SocketHandle>&& pSocket) -> void;

    // Query
    auto GetSocketAddressFamily(const ResPtr<SocketHandle>& pSocket) -> AddressFamily;
    auto GetSocketProtocol(const ResPtr<SocketHandle>& pSocket) -> Protocol;

    // Error handle
    auto GetSystemLastError() -> int;
    auto GetSocketLastError(const ResPtr<SocketHandle>& pSocket) -> int;

    // Client
    template<AddressFamily addrFamily>
    auto Connect(const ResPtr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint, int timeOutInMs = -1) -> State;

    // Server
    template<AddressFamily addrFamily>
    auto Bind(const ResPtr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint) -> State;
    auto Listen(const ResPtr<SocketHandle>& pSocket) -> State;
    auto Accept(const ResPtr<SocketHandle>& pSocket, int timeOutInMs = -1) -> std::pair<State, ResPtr<SocketHandle>>;

    // Send & Recv
    auto Send(const ResPtr<SocketHandle>& pSocket, const char* pDataBuffer, int bufferSize) -> State;
    auto Receive(const ResPtr<SocketHandle>& pSocket, char* pDataBuffer, int bufferSize, int timeOutInMs = -1) -> std::pair<State, int>;

#pragma region [template impl]

    template <AddressFamily addrFamily>
    State Connect(const ResPtr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint, int timeOutInMs)
    {
        ASSERT_MSG(false, "AddressFamily Error");
        return State::SystemError;
    }

    template <AddressFamily addrFamily>
    State Bind(const ResPtr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint)
    {
        ASSERT_MSG(false, "AddressFamily Error");
        return State::SystemError;
    }

#pragma endregion
}