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
        struct Result
        {
            std::vector<IpV4> v4Addr;
            std::vector<IpV6> v6Addr;
        };

        // Support both name & addr
        static Result GetIpAddress(const std::string& str);
        static Result GetLocalIpAddress();
        static std::string GetHostName();

    private:
        static Result GetIpAddressByName(const std::string& str);
    };
}