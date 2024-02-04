#pragma once

#include "Network.h"
#include <memory>

namespace Helper
{
    struct SocketHandle;

    class Socket
    {
    public:
        Socket() = delete;

    public:
        enum class State
        {
            Success,
            Timeout,
            SystemError,
            SocketError,
            WsaVersionError,
            InvalidSocket,
            AddressFamilyNotMatch,
            ConnectionFailed,
        };

        enum class AddressFamily
        {
            IpV4,
            IpV6
        };

        enum class Protocol
        {
            Tcp,
            Udp
        };

    public:
        // Init and clean
        static auto IsInitialized() -> bool;
        static auto InitEnvironment() -> State;
        static auto DestroyEnvironment() -> void;

        // Create and destroy
        static auto Create(AddressFamily family, Protocol protocol) -> std::unique_ptr<SocketHandle>;
        static auto Destroy(std::unique_ptr<SocketHandle>&& pSocket) -> void;
        static auto GetSocketAddressFamily(const std::unique_ptr<SocketHandle>& pSocket) -> AddressFamily;
        static auto GetSocketProtocol(const std::unique_ptr<SocketHandle>& pSocket) -> Protocol;

        // Error handle
        static auto GetSystemLastError() -> int;
        static auto GetSocketLastError(const std::unique_ptr<SocketHandle>& pSocket) -> int;

        // Client
        static auto Connect(const std::unique_ptr<SocketHandle>& pSocket, const NetEndPointV4& endpoint, int timeOutInMs = -1) -> State;
        static auto Connect(const std::unique_ptr<SocketHandle>& pSocket, const NetEndPointV6& endpoint, int timeOutInMs = -1) -> State;

        // Server


    private:
        inline static bool _socketInit = false;
    };
}