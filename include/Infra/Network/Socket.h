#pragma once

#include "EndPoint.h"

namespace Infra
{
    class Socket
    {
    public:
        enum class Protocol
        {
            TCP, UDP
        };

    public:
        void SetBlocking(bool block);

    public:
        static std::optional<Socket> Create(AddressFamily af = AddressFamily::IpV4, Protocol protocol = Protocol::TCP);

    private:
        Socket(AddressFamily af, Protocol protocol, void* handle);

    private:
        Protocol _protocol;
        AddressFamily _af;
        void* _nativeHandle;
        bool _isBlocking;
    };
}