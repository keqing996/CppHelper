#pragma once

#include "IpAddress.h"

namespace Infra
{
    template<AddressFamily>
    struct EndPoint { };

    template<>
    struct EndPoint<AddressFamily::IpV4>
    {
        IpV4 ip;
        uint16_t port;
    };

    template<>
    struct EndPoint<AddressFamily::IpV6>
    {
        IpV6 ip;
        uint16_t port;
        uint32_t scopeId;
    };

    using EndPointV4 = EndPoint<AddressFamily::IpV4>;
    using EndPointV6 = EndPoint<AddressFamily::IpV6>;


}
