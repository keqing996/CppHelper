#include "../../Socket.h"
#include "../../PlatformDefine.h"
#include "../../Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include <WinSock2.h>
#include <ws2ipdef.h>
#include "../../ScopeGuard.h"

#pragma comment(lib, "Ws2_32.lib")

namespace Helper
{
    struct SocketHandle
    {
        SOCKET handle;
        Socket::AddressFamily family;
        Socket::Protocol protocol;
        int lastError;
    };

    static int GetWsaAddressFamily(Socket::AddressFamily family)
    {
        switch (family)
        {
        case Socket::AddressFamily::IpV4:
            return AF_INET;
        case Socket::AddressFamily::IpV6:
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

    static SOCKADDR_IN GetSocketAddrIpV4(const std::unique_ptr<SocketHandle>& pSocket, const NetEndPointV4& endpoint)
    {
        SOCKADDR_IN serverAddr {};
        serverAddr.sin_family = GetWsaAddressFamily(Socket::GetSocketAddressFamily(pSocket));
        serverAddr.sin_port = ::htons(endpoint.GetPort());
        serverAddr.sin_addr.S_un.S_addr = ::htonl(endpoint.GetIp());
        return serverAddr;
    }

    static SOCKADDR_IN6 GetSocketAddrIpV6(const std::unique_ptr<SocketHandle>& pSocket, const NetEndPointV6& endpoint)
    {
        SOCKADDR_IN6 serverAddr {};
        serverAddr.sin6_family = GetWsaAddressFamily(Socket::GetSocketAddressFamily(pSocket));
        ::memcpy(&serverAddr.sin6_addr, endpoint.GetIp().data(), NetEndPointV6::ADDR_SIZE);
        serverAddr.sin6_scope_id = endpoint.GetScopeId();
        serverAddr.sin6_port = endpoint.GetPort();
        return serverAddr;
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

        // Set socket non-blocking
        u_long mode = 1;
        const int setAsyncResult = ioctlsocket(socket, FIONBIO, &mode);
        if (setAsyncResult != 0)
        {
            ::closesocket(socket);
            return nullptr;
        }

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

    auto Socket::GetSocketAddressFamily(const std::unique_ptr<SocketHandle>& pSocket) -> AddressFamily
    {
        if (pSocket == nullptr)
            return AddressFamily::IpV4;

        return pSocket->family;
    }

    auto Socket::GetSocketProtocol(const std::unique_ptr<SocketHandle>& pSocket) -> Protocol
    {
        if (pSocket == nullptr)
            return Protocol::Tcp;

        return pSocket->protocol;
    }

    auto Socket::GetSystemLastError() -> int
    {
        return ::WSAGetLastError();
    }

    auto Socket::GetSocketLastError(const std::unique_ptr<SocketHandle>& pSocket) -> int
    {
        if (pSocket == nullptr)
            return 0;

        return pSocket->lastError;
    }

    static Socket::State ConnectInternal(const std::unique_ptr<SocketHandle>& pSocket, const SOCKADDR* pAddr, int size, int timeOutInMs)
    {
        const auto connectResult = ::connect(pSocket->handle, pAddr , size);

        // If not socket error, means connect success immediately, no need to poll.
        if (connectResult != SOCKET_ERROR)
            return Socket::State::Success;

        // If last error not BLOCK or INPROGRESS, means an error really happened.
        const int lastError = ::WSAGetLastError();
        if (lastError != WSAEINPROGRESS && lastError != WSAEWOULDBLOCK)
        {
            pSocket->lastError = lastError;
            return Socket::State::SocketError;
        }

        // Poll wait, timeout = -1 means infinity.
        WSAPOLLFD pollFd;
        pollFd.fd = pSocket->handle;
        pollFd.events = POLLOUT;

        if (WSAPoll(&pollFd, 1, timeOutInMs) == SOCKET_ERROR)
        {
            pSocket->lastError = lastError;
            return Socket::State::SocketError;
        }

        // Check poll event
        if (pollFd.revents & POLLOUT == 0)
            return Socket::State::ConnectionFailed;

        return Socket::State::Success;
    }

    auto Socket::Connect(const std::unique_ptr<SocketHandle>& pSocket, const NetEndPointV4& endpoint, int timeOutInMs) -> State
    {
        if (pSocket == nullptr)
            return State::InvalidSocket;

        const auto socketFamily = GetSocketAddressFamily(pSocket);
        if (socketFamily != AddressFamily::IpV4)
            return State::AddressFamilyNotMatch;

        SOCKADDR_IN serverAddr = GetSocketAddrIpV4(pSocket, endpoint);
        return ConnectInternal(pSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN), timeOutInMs);
    }

    auto Socket::Connect(const std::unique_ptr<SocketHandle>& pSocket, const NetEndPointV6& endpoint, int timeOutInMs) -> State
    {
        if (pSocket == nullptr)
            return State::InvalidSocket;

        const auto socketFamily = GetSocketAddressFamily(pSocket);
        if (socketFamily != AddressFamily::IpV6)
            return State::AddressFamilyNotMatch;

        SOCKADDR_IN6 serverAddr = GetSocketAddrIpV6(pSocket, endpoint);
        return ConnectInternal(pSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN6), timeOutInMs);
    }

    auto Socket::Bind(const std::unique_ptr<SocketHandle>& pSocket, const NetEndPointV4& endpoint) -> State
    {
        if (pSocket == nullptr)
            return State::InvalidSocket;

        const auto socketFamily = GetSocketAddressFamily(pSocket);
        if (socketFamily != AddressFamily::IpV4)
            return State::AddressFamilyNotMatch;

        SOCKADDR_IN serverAddr = GetSocketAddrIpV4(pSocket, endpoint);
        const auto bindResult = ::bind(pSocket->handle, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN));
        if (bindResult == SOCKET_ERROR)
        {
            pSocket->lastError = ::WSAGetLastError();
            return State::SocketError;
        }

        return State::Success;
    }

    auto Socket::Bind(const std::unique_ptr<SocketHandle>& pSocket, const NetEndPointV6& endpoint) -> State
    {
        if (pSocket == nullptr)
            return State::InvalidSocket;

        const auto socketFamily = GetSocketAddressFamily(pSocket);
        if (socketFamily != AddressFamily::IpV6)
            return State::AddressFamilyNotMatch;

        SOCKADDR_IN6 serverAddr = GetSocketAddrIpV6(pSocket, endpoint);
        const auto bindResult = ::bind(pSocket->handle, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN6));
        if (bindResult == SOCKET_ERROR)
        {
            pSocket->lastError = ::WSAGetLastError();
            return State::SocketError;
        }

        return State::Success;
    }

    auto Socket::Listen(const std::unique_ptr<SocketHandle>& pSocket) -> State
    {
        if (pSocket == nullptr)
            return State::InvalidSocket;

        const auto result = ::listen(pSocket->handle, SOMAXCONN);
        if (result == SOCKET_ERROR)
        {
            pSocket->lastError = ::WSAGetLastError();
            return State::SocketError;
        }

        return State::Success;
    }
}

#endif