#pragma once

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <optional>

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
            SystemError,
            WsaVersionError,
        };

        enum class AddressFamily
        {
            IpV4,
            Ipv6
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


    private:
        inline static bool _socketInit = false;
    };
}