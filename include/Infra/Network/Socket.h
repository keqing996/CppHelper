#pragma once

#include "Infra/Network/SocketState.h"
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

        // Select
        static SocketState SelectRead(const Socket* pSocket, int timeoutInMs = -1);
        static SocketState SelectWrite(const Socket* pSocket, int timeoutInMs = -1);
        SocketState SelectRead(int timeoutInMs = -1);
        SocketState SelectWrite(int timeoutInMs = -1);

        // Connect an endpoint.
        SocketState Connect(const EndPoint& endpoint, int timeOutInMs = -1);

        // Disconnect
        virtual void Disconnect();

        // Get remote ip
        std::optional<IpAddress> GetRemoteIpAddress() const;

    public:
        static std::optional<Socket> Create(IpAddress::Family af = IpAddress::Family::IpV4, Protocol protocol = Protocol::TCP);

    private:
        Socket(IpAddress::Family af, Protocol protocol, void* handle);

    private:
        Protocol _protocol;
        IpAddress::Family _addressFamily;
        void* _handle;
        bool _isBlocking;
    };
}