#include "Infra/Network/Socket.h"

#include "Windows/WindowsSocket.h"
#include "Posix/PosixSocket.h"

namespace Infra
{
    static int GetAddressFamily(IpAddress::Family family)
    {
        switch (family)
        {
            case IpAddress::Family::IpV4:
                return AF_INET;
            case IpAddress::Family::IpV6:
                return AF_INET6;
        }

        return AF_INET;
    }

    static std::pair<int, int> GetProtocol(Socket::Protocol protocol)
    {
        switch (protocol)
        {
            case Socket::Protocol::TCP:
                return std::make_pair(IPPROTO_TCP, SOCK_STREAM);
            case Socket::Protocol::UDP:
                return std::make_pair(IPPROTO_UDP, SOCK_DGRAM);
        }

        return std::make_pair(IPPROTO_TCP, SOCK_STREAM);
    }

    std::optional<Socket> Socket::Create(IpAddress::Family af, Protocol protocol)
    {
        auto addressFamily = GetAddressFamily(af);
        auto [wsaSocketType, wsaProtocol] = GetProtocol(protocol);
        const SocketHandle handle = ::socket(addressFamily, wsaSocketType, wsaProtocol);
        if (handle == Device::GetInvalidSocket())
            return std::nullopt;

        Socket socket(af, protocol, Device::ToGeneralHandle(handle));
        socket.SetBlocking(true);

        return socket;
    }

    Socket::Socket(IpAddress::Family af, Protocol protocol, void* handle)
        : _protocol(protocol)
        , _af(af)
        , _handle(handle)
        , _isBlocking(false)
    {
    }

    void* Socket::GetNativeHandle() const
    {
        return _handle;
    }

    bool Socket::IsBlocking() const
    {
        return _isBlocking;
    }

    bool Socket::SetBlocking(bool block)
    {
        return Device::SetSocketBlocking(_handle, block);
    }
}
