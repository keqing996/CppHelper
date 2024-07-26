#include "Infra/Network/Socket.h"

#include "Windows/WindowsNetworkHeader.h"
#include "Posix/PosixNetworkHeader.h"

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

    std::optional<Socket> Socket::Create(AddressFamily af, Protocol protocol)
    {
        auto addressFamily = GetAddressFamily(af);
        auto [wsaSocketType, wsaProtocol] = GetProtocol(protocol);
        const auto handle = ::socket(addressFamily, wsaSocketType, wsaProtocol);
        if (handle == GetInvalidSocket())
            return std::nullopt;

        Socket socket(af, protocol, ToGeneralHandle(handle));
        socket.SetBlocking(true);

        return socket;
    }

    Socket::Socket(AddressFamily af, Protocol protocol, void* handle)
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
}
