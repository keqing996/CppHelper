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
        bool IsBlocking() const;
        bool SetBlocking(bool block);
        void Close();
        void* GetNativeHandle() const;

    public:
        static std::optional<Socket> Create(AddressFamily af = AddressFamily::IpV4, Protocol protocol = Protocol::TCP);

    private:
        Socket(AddressFamily af, Protocol protocol, void* handle);

    private:
        Protocol _protocol;
        AddressFamily _af;
        void* _handle;
        bool _isBlocking;
    };
}