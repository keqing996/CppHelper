#pragma once

#include <cstdint>
#include <array>

namespace Infra::Socket
{
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
    class EndPoint
    {
    };

    template<>
    class EndPoint<AddressFamily::IpV4>
    {
    public:
        EndPoint(uint32_t ip, uint16_t port)
            : _ip(ip)
            , _port(port)
        {
        }

    public:
        auto GetIp() const -> uint32_t
        {
            return _ip;
        }

        auto GetPort() const -> uint16_t
        {
            return _port;
        }

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
        EndPoint(const uint8_t* array, uint16_t port, uint32_t scopeId)
            : _port(port)
            , _scopeId(scopeId)
        {
            ::memcpy(_ip.data(), array, ADDR_SIZE);
        }

    public:
        auto GetIp() const -> const std::array<uint8_t, ADDR_SIZE>&
        {
            return _ip;
        }

        auto GetPort() const -> uint16_t
        {
            return _port;
        }

        auto GetScopeId() const -> uint32_t
        {
            return _scopeId;
        }

    private:
        std::array<uint8_t, ADDR_SIZE> _ip{};
        uint16_t _port;
        uint32_t _scopeId;
    };

    using EndPointV4 = EndPoint<AddressFamily::IpV4>;
    using EndPointV6 = EndPoint<AddressFamily::IpV6>;
}