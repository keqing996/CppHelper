#pragma once

#include "Infra/Network/SocketState.h"
#include "EndPoint.h"

namespace Infra
{
    class Socket
    {
    public:
        virtual ~Socket() = default;

        // Return is socket in blocking mode.
        bool IsBlocking() const;

        // Set socket blocking mode.
        bool SetBlocking(bool block, bool force = false);

        // Close socket.
        void Close();

        // Return native handle by platform, SOCKET on widnows, int on posix plat.
        void* GetNativeHandle() const;

        // Select
        static SocketState SelectRead(const Socket* pSocket, int timeoutInMs = -1);
        static SocketState SelectWrite(const Socket* pSocket, int timeoutInMs = -1);
        SocketState SelectRead(int timeoutInMs = -1);
        SocketState SelectWrite(int timeoutInMs = -1);

    protected:
        Socket(IpAddress::Family af, void* handle);

    protected:
        IpAddress::Family _addressFamily;
        void* _handle;
        bool _isBlocking;
    };
}