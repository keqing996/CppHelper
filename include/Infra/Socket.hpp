#pragma once

#include <cstdint>
#include <array>

namespace Infra
{
    class Socket
    {
    public:
        Socket() = delete;

    public:
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

        template<AddressFamily>
        class EndPoint { };

        template<>
        class EndPoint<AddressFamily::IpV4>
        {
        public:
            EndPoint(uint32_t ip, uint16_t port);

        public:
            uint32_t GetIp() const;
            uint16_t GetPort() const;

        private:
            uint32_t _ip;
            uint16_t _port;
        };

        template<>
        class EndPoint<AddressFamily::IpV6>
        {
        public:
            static int constexpr ADDR_SIZE = 16;

        public:
            EndPoint(const uint8_t* array, uint16_t port, uint32_t scopeId);

        public:
            const std::array<uint8_t, ADDR_SIZE>& GetIp() const;
            uint16_t GetPort() const;
            uint32_t GetScopeId() const;

        private:
            std::array<uint8_t, ADDR_SIZE> _ip{};
            uint16_t _port;
            uint32_t _scopeId;
        };

        using EndPointV4 = EndPoint<AddressFamily::IpV4>;
        using EndPointV6 = EndPoint<AddressFamily::IpV6>;

        struct SocketHandle
        {
            void* handle = nullptr;
            AddressFamily family;
            Protocol protocol;
        };

    public:
        // Init and clear
        static auto IsInitialized() -> bool;
        static auto InitEnvironment() -> bool;
        static auto DestroyEnvironment() -> void;

        // Create and destroy
        static auto Create(AddressFamily family, Protocol protocol) -> std::optional<SocketHandle>;
        static auto Destroy(const SocketHandle& socketHandle) -> void;

        // Client
        template<AddressFamily addrFamily>
        static auto Connect(const SocketHandle& socketHandle, const EndPoint<addrFamily>& endpoint, int timeOutInMs = -1) -> bool;

        // Server
        template<AddressFamily addrFamily>
        static auto Bind(const SocketHandle& socketHandle, const EndPoint<addrFamily>& endpoint) -> bool;
        static auto Listen(const SocketHandle& socketHandle) -> bool;
        static auto Accept(const SocketHandle& socketHandle, int timeOutInMs = -1) -> std::optional<SocketHandle>;

        // Send & Recv
        static auto Send(const SocketHandle& socketHandle, const char* pDataBuffer, int bufferSize) -> bool;
        static auto Receive(const SocketHandle& socketHandle, char* pDataBuffer, int bufferSize, int timeOutInMs = -1) -> std::optional<int>;

    };
}