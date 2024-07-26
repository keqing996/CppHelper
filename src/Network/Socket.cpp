#include "Infra/Network/Socket.h"

#include "Windows/WindowsNetworkHeader.h"
#include "Posix/PosixNetworkHeader.h"

#define ToGeneralHandle(x) reinterpret_cast<void*>(x)
#define ToNativeHandle(x) reinterpret_cast<SocketHanle>(x)

namespace Infra
{
    static int GetAddressFamily(AddressFamily family)
    {
        switch (family)
        {
            case AddressFamily::IpV4:
                return AF_INET;
            case AddressFamily::IpV6:
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

    void Socket::SetBlocking(bool block)
    {
        u_long blocking = block ? 0 : 1;
        ::ioctlsocket(ToNativeHandle(_nativeHandle), static_cast<long>(FIONBIO), &blocking);
    }

    std::optional<Socket> Socket::Create(AddressFamily af, Protocol protocol)
    {
        auto addressFamily = GetAddressFamily(af);
        auto [wsaSocketType, wsaProtocol] = GetProtocol(protocol);
        const SocketHanle handle = ::socket(addressFamily, wsaSocketType, wsaProtocol);
        if (handle == GetInvalidSocket())
            return std::nullopt;

        Socket socket(af, protocol, ToGeneralHandle(handle));
        socket.SetBlocking(true);

        return socket;
    }

    Socket::Socket(AddressFamily af, Protocol protocol, void* handle)
        : _protocol(protocol)
        , _af(af)
        , _nativeHandle(handle)
        , _isBlocking(false)
    {
    }
}
