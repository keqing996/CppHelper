
#include "Infra/PlatformDefine.h"

#if PLATFORM_SUPPORT_POSIX

#include "Infra/Network/IpAddress.h"

#include <cstring>
#include <arpa/inet.h>

namespace Infra
{
    std::uint32_t IpAddress<AddressFamily::IpV4>::HostToNetwork(std::uint32_t value)
    {
        return ::htonl(value);
    }

    std::uint32_t IpAddress<AddressFamily::IpV4>::NetworkToHost(std::uint32_t value)
    {
        return ::ntohl(value);
    }

    std::optional<IpAddress<AddressFamily::IpV4>> IpAddress<AddressFamily::IpV4>::TryParse(const std::string& str)
    {
        in_addr destinationIP {};
        auto result = ::inet_pton(AF_INET, str.c_str(), &destinationIP);
        if (result != 1)
            return std::nullopt;

        return IpAddress{ destinationIP.s_addr };
    }

    std::optional<IpAddress<AddressFamily::IpV6>> IpAddress<AddressFamily::IpV6>::TryParse(const std::string& str)
    {
        in6_addr destinationIP {};
        auto result = ::inet_pton(AF_INET6, str.c_str(), &destinationIP);
        if (result != 1)
            return std::nullopt;

        return IpAddress{ reinterpret_cast<const std::uint8_t*>(&destinationIP.__in6_u.__u6_addr16) };
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