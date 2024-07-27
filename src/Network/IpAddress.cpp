
#include "Infra/Network/IpAddress.h"
#include "Posix/PosixNetworkHeader.h"
#include "Windows/WindowsNetworkHeader.h"

#include <cstring>

namespace Infra
{

    IpAddress IpAddress::V4_ANY (0, 0, 0, 0);
    IpAddress IpAddress::V4_BROADCAST (255, 255, 255, 255);
    IpAddress IpAddress::V4_LOCAL_HOST (127, 0,0 , 1);



    /*
    IpAddress<AddressFamily::IpV4>::IpAddress LOCAL_HOST (127, 0, 0, 1);
    IpAddress<AddressFamily::IpV4>::IpAddress ANY (0, 0, 0, 0);
    IpAddress<AddressFamily::IpV4>::IpAddress BROADCAST (255, 255, 255, 255);

    IpAddress<AddressFamily::IpV4>::IpAddress() : _address(0)
    {
    }

    IpAddress<AddressFamily::IpV4>::IpAddress(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4)
    {
        _address = static_cast<uint32_t>(byte1) << 24;
        _address |= static_cast<uint32_t>(byte2) << 16;
        _address |= static_cast<uint32_t>(byte3) << 8;
        _address |= static_cast<uint32_t>(byte4);
        _address = ::htonl(_address);
    }

    IpAddress<AddressFamily::IpV4>::IpAddress(uint32_t hostOrderIp)
    {
        _address = ::htonl(hostOrderIp);
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

        return IpAddress{ reinterpret_cast<const uint8_t*>(&destinationIP.s6_addr) };
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

        ::memcpy(&address.s6_addr, _address.data(), ADDR_SIZE_BYTE);

        char resultBuffer[INET6_ADDRSTRLEN];

        ::inet_ntop(AF_INET, &address, resultBuffer, INET6_ADDRSTRLEN);

        return resultBuffer;
    }

    bool IpAddress<AddressFamily::IpV4>::operator==(const IpAddress& left, const IpAddress& right) const
    {
        return left._address == right._address;
    }

    bool IpAddress<AddressFamily::IpV4>::operator!=(const IpAddress& left, const IpAddress& right) const
    {
        return left._address != right._address;
    }

    IpAddress<AddressFamily::IpV6>::IpAddress(const uint8_t* pAddr)
    {
        ::memcpy(_address.data(), pAddr, ADDR_SIZE_BYTE);
    }

    bool IpAddress<AddressFamily::IpV6>::operator==(const IpAddress& left, const IpAddress& right) const
    {
        return left._address == right._address;
    }

    bool IpAddress<AddressFamily::IpV6>::operator!=(const IpAddress& left, const IpAddress& right) const
    {
        return left._address != right._address;
    }

    IpAddress::IpAddress(uint32_t hostOrderIp)
    {
    }
    */
    IpAddress::IpAddress(uint32_t hostOrderIp)
        : _addrFamily(Family::IpV4)
    {
        _data.ipV4Data = ::htonl(hostOrderIp);
    }

    IpAddress::IpAddress(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4)
        : _addrFamily(Family::IpV4)
    {
        uint32_t hostOrderIp = static_cast<uint32_t>(byte1) << 24;
        hostOrderIp |= static_cast<uint32_t>(byte2) << 16;
        hostOrderIp |= static_cast<uint32_t>(byte3) << 8;
        hostOrderIp |= static_cast<uint32_t>(byte4);
        _data.ipV4Data = ::htonl(hostOrderIp);
    }

    IpAddress::IpAddress(const uint8_t* pAddr)
        : _addrFamily(Family::IpV6)
    {
        ::memcpy(_data.ipV6Data, pAddr, IPV6_ADDR_SIZE_BYTE);
    }

    bool IpAddress::operator==(const IpAddress& left, const IpAddress& right) const
    {
        if (left._addrFamily != right._addrFamily)
            return false;

        switch (left._addrFamily)
        {
            case Family::IpV4:
            {
                return left._data.ipV4Data == right._data.ipV4Data;
            }
            case Family::IpV6:
            {
                const auto* leftData = reinterpret_cast<const uint64_t*>(left._data.ipV6Data);
                const auto* rightData = reinterpret_cast<const uint64_t*>(right._data.ipV6Data);
                return leftData[0] == rightData[0] && leftData[1] == rightData[1];
            }
        }

        return false;
    }

    bool IpAddress::operator!=(const IpAddress& left, const IpAddress& right) const
    {
        return !(left == right);
    }

    IpAddress::Family IpAddress::GetFamily() const
    {
    }

    std::string IpAddress::ToString() const
    {
    }

    std::optional<IpAddress> IpAddress::TryParse(const std::string& str)
    {
    }
}
