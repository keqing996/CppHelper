#pragma once

#include "Socket.h"

namespace Infra
{
    class TcpSocket: public Socket
    {
    public:
        static std::optional<Socket> Create(IpAddress::Family af = IpAddress::Family::IpV4);

    public:
        // Connect an endpoint.
        SocketState Connect(const EndPoint& endpoint, int timeOutInMs = -1);

        // Disconnect
        void Disconnect();

        // Get remote ip & port
        std::optional<EndPoint> GetRemoteEndpoint() const;

        // Send
        std::pair<SocketState, size_t> Send(void* pData, size_t size);
        std::pair<SocketState, size_t> Receive(void* pBuffer, size_t size);

    private:
        TcpSocket(IpAddress::Family af, void* handle);
    };
}