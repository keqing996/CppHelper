#pragma once

#include <memory>

#include "PlatformDefine.h"
#include "SocketGeneral.h"

namespace Helper::Socket
{
    struct SocketHandle;

    // Init and clean
    auto IsInitialized() -> bool;
    auto InitEnvironment() -> State;
    auto DestroyEnvironment() -> void;

    // Create and destroy
    auto Create(AddressFamily family, Protocol protocol) -> std::unique_ptr<SocketHandle>;
    auto Destroy(std::unique_ptr<SocketHandle>&& pSocket) -> void;

    // Query
    auto GetSocketAddressFamily(const std::unique_ptr<SocketHandle>& pSocket) -> AddressFamily;
    auto GetSocketProtocol(const std::unique_ptr<SocketHandle>& pSocket) -> Protocol;

    // Error handle
    auto GetSystemLastError() -> int;
    auto GetSocketLastError(const std::unique_ptr<SocketHandle>& pSocket) -> int;

    // Client
    template<AddressFamily addrFamily>
    auto Connect(const std::unique_ptr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint, int timeOutInMs = -1) -> State;

    // Server
    template<AddressFamily addrFamily>
    auto Bind(const std::unique_ptr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint) -> State;
    auto Listen(const std::unique_ptr<SocketHandle>& pSocket) -> State;
    auto Accept(const std::unique_ptr<SocketHandle>& pSocket, int timeOutInMs = -1) -> std::pair<State, std::unique_ptr<SocketHandle>>;

    // Send & Recv

#pragma region [template impl]

    template <AddressFamily addrFamily>
    State Connect(const std::unique_ptr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint, int timeOutInMs)
    {
        ASSERT_MSG(false, "AddressFamily Error");
        return State::SystemError;
    }

    template <AddressFamily addrFamily>
    State Bind(const std::unique_ptr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint)
    {
        ASSERT_MSG(false, "AddressFamily Error");
        return State::SystemError;
    }

#pragma endregion
}