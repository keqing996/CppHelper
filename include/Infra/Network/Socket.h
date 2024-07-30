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
        // Return is socket in blocking mode.
        bool IsBlocking() const;

        // Set socket blocking mode.
        bool SetBlocking(bool block);

        // Close socket.
        void Close();

        // Return native handle by platform, SOCKET on widnows, int on posix plat.
        void* GetNativeHandle() const;

        // Connect an endpoint.
        bool Connect(const EndPoint& endpoint, int timeOutInMs = -1);


    public:
        static std::optional<Socket> Create(IpAddress::Family af = IpAddress::Family::IpV4, Protocol protocol = Protocol::TCP);

    private:
        Socket(IpAddress::Family af, Protocol protocol, void* handle);

    private:
        Protocol _protocol;
        IpAddress::Family _af;
        void* _handle;
        bool _isBlocking;
    };
}