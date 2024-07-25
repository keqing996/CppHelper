
#include "Infra/Network/IpAddress.h"

#include <cstring>

namespace Infra
{
    IpAddress<AddressFamily::IpV4>::IpAddress LOCAL_HOST (127, 0, 0, 1);
    IpAddress<AddressFamily::IpV4>::IpAddress ANY (0, 0, 0, 0);
    IpAddress<AddressFamily::IpV4>::IpAddress BROADCAST (255, 255, 255, 255);

    IpAddress<AddressFamily::IpV4>::IpAddress() : _address(0)
    {
    }

    bool IpAddress<AddressFamily::IpV4>::operator==(const IpAddress<AddressFamily::IpV4>& left, const IpAddress<AddressFamily::IpV4>& right) const
    {
        return left._address == right._address;
    }

    bool IpAddress<AddressFamily::IpV4>::operator!=(const IpAddress<AddressFamily::IpV4>& left, const IpAddress<AddressFamily::IpV4>& right) const
    {
        return left._address != right._address;
    }

    IpAddress<AddressFamily::IpV6>::IpAddress(const uint8_t* pAddr)
    {
        ::memcpy(_address.data(), pAddr, ADDR_SIZE);
    }

    bool IpAddress<AddressFamily::IpV6>::operator==(const IpAddress<AddressFamily::IpV6>& left, const IpAddress<AddressFamily::IpV6>& right) const
    {
        return left._address == right._address;
    }

    bool IpAddress<AddressFamily::IpV6>::operator!=(const IpAddress<AddressFamily::IpV6>& left, const IpAddress<AddressFamily::IpV6>& right) const
    {
        return left._address != right._address;
    }

}
