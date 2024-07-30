#pragma once

#include "EndPoint.h"

#include <vector>

namespace Infra
{
    class DNS
    {
    public:
        DNS() = delete;

    public:
        // Support both name & addr
        static std::vector<IpAddress> GetIpAddress(const std::string& str);
        static std::vector<IpAddress> GetLocalIpAddress();
        static std::string GetHostName();

    private:
        static std::vector<IpAddress> GetIpAddressByName(const std::string& str);
    };
}