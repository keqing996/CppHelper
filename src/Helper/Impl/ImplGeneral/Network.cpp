#include "../../Network.h"

namespace Helper
{
    NetEndPointV4::NetEndPointV4(uint32_t ip, uint16_t port)
        : _ip(ip)
        , _port(port)
    {
    }

    uint32_t NetEndPointV4::GetIp() const
    {
        return _ip;
    }

    uint16_t NetEndPointV4::GetPort() const
    {
        return _port;
    }

    NetEndPointV6::NetEndPointV6(const uint8_t* array, uint16_t port, uint32_t scopeId)
        : _port(port)
        , _scopeId(scopeId)
    {
        ::memcpy(_ip.data(), array, ADDR_SIZE);
    }

    const std::array<uint8_t, NetEndPointV6::ADDR_SIZE>& NetEndPointV6::GetIp() const
    {
        return _ip;
    }

    uint16_t NetEndPointV6::GetPort() const
    {
        return _port;
    }

    auto NetEndPointV6::GetScopeId() const -> uint32_t
    {
        return _scopeId;
    }
}
