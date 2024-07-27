#pragma once

#include <array>
#include <string>
#include <cstdint>
#include <optional>

namespace Infra
{
    class IpAddress
    {
    public:
        enum class Family
        {
            IpV4,
            IpV6
        };

        static int constexpr IPV6_ADDR_SIZE_BYTE = 16;

    public:
        // V4 constructor
        explicit IpAddress(uint32_t hostOrderIp);
        IpAddress(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4);

        // V6 constructor
        explicit IpAddress(const uint8_t* pAddr);

        // operator
        bool operator==(const IpAddress& left, const IpAddress& right) const;
        bool operator!=(const IpAddress& left, const IpAddress& right) const;

    public:
        Family GetFamily() const;
        std::string ToString() const;

    public:
        static IpAddress V4_LOCAL_HOST;
        static IpAddress V4_ANY;
        static IpAddress V4_BROADCAST;

        static std::optional<IpAddress> TryParse(const std::string& str);

    private:
        union IpData
        {
            uint32_t ipV4Data; // network order
            uint8_t ipV6Data[IPV6_ADDR_SIZE_BYTE];
        };

        Family _addrFamily;
        IpData _data;
    };

    /*
    template<>
    class IpAddress<AddressFamily::IpV4>
    {
    public:
        IpAddress();
        explicit IpAddress(uint32_t hostOrderIp);
        IpAddress(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4);

        bool operator==(const IpAddress& left, const IpAddress& right) const;
        bool operator!=(const IpAddress& left, const IpAddress& right) const;

    public:
        std::string ToString() const;

    public:
        static IpAddress LOCAL_HOST;
        static IpAddress ANY;
        static IpAddress BROADCAST;

        static std::optional<IpAddress> TryParse(const std::string& str);

    private:
        // network order
        uint32_t _address;
    };

    template<>
    class IpAddress<AddressFamily::IpV6>
    {
    public:
        explicit IpAddress(const uint8_t* pAddr);

        bool operator==(const IpAddress& left, const IpAddress& right) const;
        bool operator!=(const IpAddress& left, const IpAddress& right) const;

    public:
        std::string ToString() const;

    public:
        static int constexpr ADDR_SIZE_BYTE = 16;
        static std::optional<IpAddress> TryParse(const std::string& str);

    private:
        // network order
        std::array<uint8_t, ADDR_SIZE_BYTE> _address{};
    };

    using IpV4 = IpAddress<AddressFamily::IpV4>;
    using IpV6 = IpAddress<AddressFamily::IpV6>;
    */
}
