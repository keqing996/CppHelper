#pragma once

#include <cstdint>
#include <array>

namespace Helper
{
    class NetEndPointV4
    {
    public:
        NetEndPointV4(uint32_t ip, uint16_t port);

    public:
        auto GetIp() const -> uint32_t;
        auto GetPort() const -> uint16_t;

    private:
        uint32_t _ip;
        uint16_t _port;
    };

    class NetEndPointV6
    {
    public:
        static int constexpr ADDR_SIZE = 16;

    public:
        NetEndPointV6(const uint8_t* array, uint16_t port);

    public:
        auto GetIp() const -> const std::array<uint8_t, ADDR_SIZE>&;
        auto GetPort() const -> uint16_t;

    private:
        std::array<uint8_t, ADDR_SIZE> _ip {};
        uint16_t _port;
    };
}
