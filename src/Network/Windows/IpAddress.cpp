
#include "Infra/PlatformDefine.h"

#if PLATFORM_WINDOWS

#include "Infra/Network/IpAddress.h"
#include "Infra/Utility/String.h"
#include "Infra/Platform/Windows/WindowsDefine.h"

#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

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
        if (FAILED(result))
            return std::nullopt;

        IpAddress ip;
        ip._address = destinationIP.S_un.S_addr;
        return ip;
    }

    std::optional<IpAddress<AddressFamily::IpV6>> IpAddress<AddressFamily::IpV6>::TryParse(const std::string& str)
    {
        in6_addr destinationIP {};
        auto result = ::inet_pton(AF_INET6, str.c_str(), &destinationIP);
        if (FAILED(result))
            return std::nullopt;

        return IpAddress{ reinterpret_cast<const uint8_t*>(&destinationIP.u.Byte) };
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

        ::memcpy(&address.u.Byte, _address.data(), ADDR_SIZE);

        char resultBuffer[INET6_ADDRSTRLEN];

        ::inet_ntop(AF_INET, &address, resultBuffer, INET6_ADDRSTRLEN);

        return resultBuffer;
    }
}

#endif