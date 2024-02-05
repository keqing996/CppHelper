#pragma once

#include <memory>

#include "PlatformDefine.h"
#include "SocketGeneral.h"

namespace Helper::Socket
{
    struct SocketHandle;

    // Init and clean
    static auto IsInitialized() -> bool;
    static auto InitEnvironment() -> State;
    static auto DestroyEnvironment() -> void;

    // Create and destroy
    static auto Create(AddressFamily family, Protocol protocol) -> std::unique_ptr<SocketHandle>;
    static auto Destroy(std::unique_ptr<SocketHandle>&& pSocket) -> void;

    // Query
    static auto GetSocketAddressFamily(const std::unique_ptr<SocketHandle>& pSocket) -> AddressFamily;
    static auto GetSocketProtocol(const std::unique_ptr<SocketHandle>& pSocket) -> Protocol;

    // Error handle
    static auto GetSystemLastError() -> int;
    static auto GetSocketLastError(const std::unique_ptr<SocketHandle>& pSocket) -> int;

    // Client
    template<AddressFamily addrFamily>
    static auto Connect(const std::unique_ptr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint, int timeOutInMs = -1) -> State;

    // Server
    template<AddressFamily addrFamily>
    static auto Bind(const std::unique_ptr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint) -> State;
    static auto Listen(const std::unique_ptr<SocketHandle>& pSocket) -> State;
    static auto Accept(const std::unique_ptr<SocketHandle>& pSocket, int timeOutInMs = -1) -> std::pair<State, std::unique_ptr<SocketHandle>>;

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