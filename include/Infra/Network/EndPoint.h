#pragma once

#include <string>
#include <cstdint>
#include <array>
#include <optional>

namespace Infra
{


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


}
