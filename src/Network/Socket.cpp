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
        , _addressFamily(af)
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

    SocketState Socket::SelectRead(int timeoutInMs)
    {
        if (timeoutInMs <= 0)
            return SocketState::Error;

        fd_set selector;
        FD_ZERO(&selector);
        FD_SET(Device::ToNativeHandle(_handle), &selector);

        timeval time{};
        time.tv_sec  = static_cast<long>(timeoutInMs / 1000);
        time.tv_usec = timeoutInMs % 1000 * 1000;

        if (select(static_cast<int>(Device::ToNativeHandle(_handle) + 1), &selector, nullptr, nullptr, &time) > 0)
            return SocketState::Success;

        return Device::GetErrorState();
    }

    SocketState Socket::SelectWrite(int timeoutInMs)
    {
        if (timeoutInMs <= 0)
            return SocketState::Error;

        fd_set selector;
        FD_ZERO(&selector);
        FD_SET(Device::ToNativeHandle(_handle), &selector);

        timeval time{};
        time.tv_sec  = static_cast<long>(timeoutInMs / 1000);
        time.tv_usec = timeoutInMs % 1000 * 1000;

        if (select(static_cast<int>(Device::ToNativeHandle(_handle) + 1), nullptr, &selector, nullptr, &time) > 0)
            return SocketState::Success;

        return Device::GetErrorState();
    }

    SocketState Socket::Connect(const EndPoint& endpoint, int timeOutInMs)
    {
        // Check address families match.
        if (endpoint.GetAddressFamily() != _addressFamily)
            return SocketState::AddrFamilyNotMatch;

        // Disconnect last.
        Disconnect();

        union SockAddr
        {
            sockaddr_in v4;
            sockaddr_in6 v6;
        };

        SockAddr address {};
        sockaddr* pSockAddr = reinterpret_cast<sockaddr*>(&address);
        int structLen;

        switch (_addressFamily)
        {
            case IpAddress::Family::IpV4:
                address.v4.sin_addr.s_addr = ::htonl(endpoint.GetIp().GetV4Addr());
                address.v4.sin_family = AF_INET;
                address.v4.sin_port = ::htons(endpoint.GetPort());
                structLen = sizeof(sockaddr_in);
                break;
            case IpAddress::Family::IpV6:
                ::memcpy(&address.v6.sin6_addr, endpoint.GetIp().GetV6Addr(), IpAddress::IPV6_ADDR_SIZE_BYTE);
                address.v6.sin6_family = AF_INET6;
                address.v6.sin6_scope_id = endpoint.GetV6ScopeId();
                address.v6.sin6_port = ::htons(endpoint.GetPort());
                structLen = sizeof(sockaddr_in6);
                break;
            default:
                return SocketState::AddrFamilyNotMatch;
        }

        if (timeOutInMs <= 0)
        {
            // No timeout case, just connect
            if (::connect(Device::ToNativeHandle(_handle), pSockAddr, structLen) == -1)
                return Device::GetErrorState();

            return SocketState::Success;
        }
        else
        {
            // Has timeout case.
            // 1. Socket is blocking, change socket to non-blocking, then connect and select.
            // 2. Socket is non-blocking,
            const bool originalBlockState = IsBlocking();

            if (originalBlockState)
                SetBlocking(false);

            // Connect once, if connection is success, no need to select.
            if (::connect(Device::ToNativeHandle(_handle), pSockAddr, structLen) >= 0)
            {
                SetBlocking(originalBlockState);
                return SocketState::Success;
            }

            SocketState currentState = Device::GetErrorState();

            // Socket is not blocking, return immediately
            if (!originalBlockState)
                return currentState;
            else
            {
                auto state = SelectWrite(timeOutInMs);
                if (state == SocketState::Success && )
            }
        }

    }

    void Socket::Disconnect()
    {
        Close();
    }

    std::optional<IpAddress> Socket::GetRemoteIpAddress() const
    {
        if (Device::ToNativeHandle(_handle) == Device::GetInvalidSocket())
            return std::nullopt;

        if (_addressFamily == IpAddress::Family::IpV4)
        {

        }
    }
}
