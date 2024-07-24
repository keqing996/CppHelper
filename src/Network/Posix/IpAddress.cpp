
#include "Infra/PlatformDefine.h"

#if PLATFORM_SUPPORT_POSIX

#include "Infra/Network/IpAddress.h"

#include <arpa/inet.h>

namespace Infra
{
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
}


#endif