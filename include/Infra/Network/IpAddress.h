#pragma once

#include <array>
#include <string>
#include <cstdint>
#include <optional>

namespace Infra
{
    enum class AddressFamily
    {
        IpV4,
        IpV6
    };

    template<AddressFamily addrFamily>
    class IpAddress
    {
    };

    template<>
    class IpAddress<AddressFamily::IpV4>
    {
    public:
        IpAddress();
        explicit IpAddress(std::uint32_t addr);
        IpAddress(std::uint8_t byte1, std::uint8_t byte2, std::uint8_t byte3, std::uint8_t byte4);

        bool operator==(const IpAddress& left, const IpAddress& right) const;
        bool operator!=(const IpAddress& left, const IpAddress& right) const;

    public:
        std::string ToString() const;

    public:
        static IpAddress LOCAL_HOST;
        static IpAddress ANY;
        static IpAddress BROADCAST;

        static std::optional<IpAddress> TryParse(const std::string& str);
        static std::uint32_t HostToNetwork(std::uint32_t value);
        static std::uint32_t NetworkToHost(std::uint32_t value);

    private:
        std::uint32_t _address;
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
        static int constexpr ADDR_SIZE = 16;
        static std::optional<IpAddress> TryParse(const std::string& str);

    private:
        std::array<uint8_t, ADDR_SIZE> _address{};
    };
}
