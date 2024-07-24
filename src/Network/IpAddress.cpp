
#include "Infra/Network/IpAddress.h"

#include <cstring>

namespace Infra
{
    static IpAddress<AddressFamily::IpV4>::IpAddress LOCAL_HOST (127, 0, 0, 1);
    static IpAddress<AddressFamily::IpV4>::IpAddress ANY (0, 0, 0, 0);
    static IpAddress<AddressFamily::IpV4>::IpAddress BROADCAST (255, 255, 255, 255);

    IpAddress<AddressFamily::IpV4>::IpAddress() : _address(0)
    {
    }

    IpAddress<AddressFamily::IpV4>::IpAddress(std::uint32_t addr) : _address(addr)
    {
    }

    IpAddress<AddressFamily::IpV4>::IpAddress(std::uint8_t byte1, std::uint8_t byte2, std::uint8_t byte3, std::uint8_t byte4)
    {
        std::uint8_t* pU8 = reinterpret_cast<std::uint8_t*>(&_address);
        pU8[0] = byte1;
        pU8[1] = byte2;
        pU8[2] = byte3;
        pU8[3] = byte4;
    }

    IpAddress<AddressFamily::IpV6>::IpAddress(const uint8_t* pAddr)
    {
        ::memcpy(_address.data(), pAddr, ADDR_SIZE);
    }


}
