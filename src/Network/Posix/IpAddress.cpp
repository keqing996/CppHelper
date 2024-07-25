
#include "Infra/PlatformDefine.h"

#if PLATFORM_SUPPORT_POSIX

#include "Infra/Network/IpAddress.h"

#include <cstring>
#include <arpa/inet.h>

namespace Infra
{
    IpAddress<AddressFamily::IpV4>::IpAddress(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4)
    {
        _address = static_cast<uint32_t>(byte1) << 24;
        _address |= static_cast<uint32_t>(byte2) << 16;
        _address |= static_cast<uint32_t>(byte3) << 8;
        _address |= static_cast<uint32_t>(byte4);
        _address = ::htonl(_address);
    }

    std::optional<IpAddress<AddressFamily::IpV4>> IpAddress<AddressFamily::IpV4>::TryParse(const std::string& str)
    {
        in_addr destinationIP {};
        auto result = ::inet_pton(AF_INET, str.c_str(), &destinationIP);
        if (result != 1)
            return std::nullopt;

        IpAddress ip;
        ip._address = destinationIP.s_addr;
        return ip;
    }

    std::optional<IpAddress<AddressFamily::IpV6>> IpAddress<AddressFamily::IpV6>::TryParse(const std::string& str)
    {
        in6_addr destinationIP {};
        auto result = ::inet_pton(AF_INET6, str.c_str(), &destinationIP);
        if (result != 1)
            return std::nullopt;

        return IpAddress{ reinterpret_cast<const uint8_t*>(&destinationIP.__in6_u.__u6_addr16) };
    }

    std::string IpAddress<AddressFamily::IpV4>::ToString() const
    {
        in_addr address{};
        address.s_addr = _address;

        char resultBuffer[INET_ADDRSTRLEN];

        ::inet_ntop(AF_INET, &address, resultBuffer, INET_ADDRSTRLEN);

        return resultBuffer;
    }

    std::string IpAddress<AddressFamily::IpV6>::ToString() const
    {
        in6_addr address{};

        ::memcpy(&address.__in6_u.__u6_addr8, _address.data(), ADDR_SIZE);

        char resultBuffer[INET6_ADDRSTRLEN];

        ::inet_ntop(AF_INET, &address, resultBuffer, INET6_ADDRSTRLEN);

        return resultBuffer;
    }
}


#endif