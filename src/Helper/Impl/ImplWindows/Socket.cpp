#include "../../Socket.h"
#include "../../PlatformDefine.h"
#include "../../Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include <WinSock2.h>
#include "../../ScopeGuard.h"

#pragma comment(lib, "Ws2_32.lib")

namespace Helper
{
    struct SocketHandle
    {
        SOCKET handle;
    };

    static int GetWsaAddressFamily(Socket::AddressFamily family)
    {
        switch (family)
        {
        case Socket::AddressFamily::IpV4:
            return AF_INET;
        case Socket::AddressFamily::Ipv6:
            return AF_INET6;
        }
    }

    static std::pair<int, int> GetWsaProtocol(Socket::Protocol protocol)
    {
        switch (protocol)
        {
        case Socket::Protocol::Tcp:
            return std::make_pair(IPPROTO_TCP, SOCK_STREAM);
        case Socket::Protocol::Udp:
            return std::make_pair(IPPROTO_UDP, SOCK_DGRAM);
        }
    }

    bool Socket::IsInitialized()
    {
        return _socketInit;
    }

    Socket::State Socket::InitEnvironment()
    {
        WORD wVersion = MAKEWORD(2, 2);
        WSADATA wsadata;

        const auto wsaStartUpResult = ::WSAStartup(wVersion, &wsadata);
        if (wsaStartUpResult != 0)
            return State::SystemError;

        if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2)
            return State::WsaVersionError;

        return State::Success;
    }

    void Socket::DestroyEnvironment()
    {
        ::WSACleanup();
    }

    std::unique_ptr<SocketHandle> Socket::Create(AddressFamily family, Protocol protocol)
    {
        const int wsaAddrFamily = GetWsaAddressFamily(family);
        const auto [wsaProtocol, wsaSocketType] = GetWsaProtocol(protocol);

        const SOCKET socket = ::socket(wsaAddrFamily, wsaSocketType, wsaProtocol);
        if (socket == INVALID_SOCKET)
            return nullptr;

        std::unique_ptr<SocketHandle> result(new SocketHandle());
        result->handle = socket;
        return result;
    }

    void Socket::Destroy(std::unique_ptr<SocketHandle>&& pSocket)
    {
        if (pSocket == nullptr)
            return;

        ::closesocket(pSocket->handle);
    }
}

#endif