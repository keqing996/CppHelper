
#include "Infra/PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "Infra/Network/IpAddress.h"
#include "Infra/Utility/String.h"
#include "Infra/Platform/Windows/WindowsDefine.h"

#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

namespace Infra
{
    std::optional<IpAddress<AddressFamily::IpV4>> IpAddress<AddressFamily::IpV4>::TryParse(const std::string& str)
    {
        in_addr destinationIP {};
        auto result = ::inet_pton(AF_INET, str.c_str(), &destinationIP);
        if (FAILED(result))
            return std::nullopt;

        return IpAddress{ static_cast<std::uint32_t>(destinationIP.S_un.S_addr) };
    }

    std::optional<IpAddress<AddressFamily::IpV6>> IpAddress<AddressFamily::IpV6>::TryParse(const std::string& str)
    {
        in6_addr destinationIP {};
        auto result = ::inet_pton(AF_INET6, str.c_str(), &destinationIP);
        if (FAILED(result))
            return std::nullopt;

        return IpAddress{ reinterpret_cast<const std::uint8_t*>(&destinationIP.u.Byte) };
    }
}

#endif