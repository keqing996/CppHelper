#include "../../SocketGeneral.h"
#include <cstring>

namespace Infra::Socket
{
    EndPoint<AddressFamily::IpV4>::EndPoint(uint32_t ip, uint16_t port)
        : _ip(ip)
        , _port(port)
    {
    }

    uint32_t EndPoint<AddressFamily::IpV4>::GetIp() const
    {
        return _ip;
    }

    uint16_t EndPoint<AddressFamily::IpV4>::GetPort() const
    {
        return _port;
    }

    EndPoint<AddressFamily::IpV6>::EndPoint(const uint8_t* array, uint16_t port, uint32_t scopeId)
        : _port(port)
        , _scopeId(scopeId)
    {
        ::memcpy(_ip.data(), array, ADDR_SIZE);
    }

    const std::array<uint8_t, EndPoint<AddressFamily::IpV6>::ADDR_SIZE>& EndPoint<AddressFamily::IpV6>::GetIp() const
    {
        return _ip;
    }

    uint16_t EndPoint<AddressFamily::IpV6>::GetPort() const
    {
        return _port;
    }

    auto EndPoint<AddressFamily::IpV6>::GetScopeId() const -> uint32_t
    {
        return _scopeId;
    }
}
