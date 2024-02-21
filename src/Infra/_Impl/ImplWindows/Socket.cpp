#include "../../Socket.h"
#include "../../PlatformDefine.h"
#include "../../Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include <WinSock2.h>
#include <ws2ipdef.h>
#include "../../ScopeGuard.h"

#pragma comment(lib, "Ws2_32.lib")

namespace Infra::Socket
{

    static bool gSocketEnvironmentInit = false;

    struct SocketHandle
    {
        SOCKET handle;
        AddressFamily family;
        Protocol protocol;
        int lastError;
    };

    static ResPtr<SocketHandle> MakeSocketHandle(SOCKET handle, AddressFamily family, Protocol protocol)
    {
        ResPtr<SocketHandle> result(new SocketHandle());
        result->handle = handle;
        result->family = family;
        result->protocol = protocol;
        result->lastError = 0;
        return result;
    }

    static int GetWsaAddressFamily(AddressFamily family)
    {
        switch (family)
        {
        case AddressFamily::IpV4:
            return AF_INET;
        case AddressFamily::IpV6:
            return AF_INET6;
        }
    }

    static std::pair<int, int> GetWsaProtocol(Protocol protocol)
    {
        switch (protocol)
        {
        case Protocol::Tcp:
            return std::make_pair(IPPROTO_TCP, SOCK_STREAM);
        case Protocol::Udp:
            return std::make_pair(IPPROTO_UDP, SOCK_DGRAM);
        }
    }

    static SOCKADDR_IN GetSocketAddrIpV4(const ResPtr<SocketHandle>& pSocket, const EndPointV4& endpoint)
    {
        SOCKADDR_IN serverAddr {};
        serverAddr.sin_family = GetWsaAddressFamily(GetSocketAddressFamily(pSocket));
        serverAddr.sin_port = ::htons(endpoint.GetPort());
        serverAddr.sin_addr.S_un.S_addr = ::htonl(endpoint.GetIp());
        return serverAddr;
    }

    static SOCKADDR_IN6 GetSocketAddrIpV6(const ResPtr<SocketHandle>& pSocket, const EndPointV6& endpoint)
    {
        SOCKADDR_IN6 serverAddr {};
        serverAddr.sin6_family = GetWsaAddressFamily(GetSocketAddressFamily(pSocket));
        ::memcpy(&serverAddr.sin6_addr, endpoint.GetIp().data(), EndPointV6::ADDR_SIZE);
        serverAddr.sin6_scope_id = endpoint.GetScopeId();
        serverAddr.sin6_port = endpoint.GetPort();
        return serverAddr;
    }

    bool IsInitialized()
    {
        return gSocketEnvironmentInit;
    }

    State InitEnvironment()
    {
        WORD wVersion = MAKEWORD(2, 2);
        WSADATA wsadata;

        const auto wsaStartUpResult = ::WSAStartup(wVersion, &wsadata);
        if (wsaStartUpResult != 0)
            return State::SystemError;

        if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2)
            return State::WsaVersionError;

        gSocketEnvironmentInit = true;
        return State::Success;
    }

    void DestroyEnvironment()
    {
        ::WSACleanup();
        gSocketEnvironmentInit = false;
    }

    ResPtr<SocketHandle> Create(AddressFamily family, Protocol protocol)
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

        return MakeSocketHandle(socket, family, protocol);
    }

    void Destroy(ResPtr<SocketHandle>&& pSocket)
    {
        if (pSocket == nullptr)
            return;

        ::closesocket(pSocket->handle);
    }

    auto GetSocketAddressFamily(const ResPtr<SocketHandle>& pSocket) -> AddressFamily
    {
        if (pSocket == nullptr)
            return AddressFamily::IpV4;

        return pSocket->family;
    }

    auto GetSocketProtocol(const ResPtr<SocketHandle>& pSocket) -> Protocol
    {
        if (pSocket == nullptr)
            return Protocol::Tcp;

        return pSocket->protocol;
    }

    auto GetSystemLastError() -> int
    {
        return ::WSAGetLastError();
    }

    auto GetSocketLastError(const ResPtr<SocketHandle>& pSocket) -> int
    {
        if (pSocket == nullptr)
            return 0;

        return pSocket->lastError;
    }

    static State ConnectInternal(const ResPtr<SocketHandle>& pSocket, const SOCKADDR* pAddr, int size, int timeOutInMs)
    {
        const auto connectResult = ::connect(pSocket->handle, pAddr , size);

        // If not socket error, means connect success immediately, no need to poll.
        if (connectResult != SOCKET_ERROR)
            return State::Success;

        // If last error not BLOCK or INPROGRESS, means an error really happened.
        const int lastError = ::WSAGetLastError();
        if (lastError != WSAEINPROGRESS && lastError != WSAEWOULDBLOCK)
        {
            pSocket->lastError = lastError;
            return State::SocketError;
        }

        // Poll wait, timeout = -1 means infinity.
        WSAPOLLFD pollFd;
        pollFd.fd = pSocket->handle;
        pollFd.events = POLLOUT;

        int pollResult = ::WSAPoll(&pollFd, 1, timeOutInMs);
        if (pollResult == SOCKET_ERROR)
        {
            pSocket->lastError = ::WSAGetLastError();
            return State::SocketError;
        }

        if (pollResult == 0)
            return State::Timeout;

        // Check poll event
        if ((pollFd.revents & POLLOUT) == 0)
            return State::ConnectionFailed;

        return State::Success;
    }

    template<>
    auto Connect<AddressFamily::IpV4>(const ResPtr<SocketHandle>& pSocket, const EndPointV4& endpoint, int timeOutInMs) -> State
    {
        if (pSocket == nullptr)
            return State::InvalidSocket;

        const auto socketFamily = GetSocketAddressFamily(pSocket);
        if (socketFamily != AddressFamily::IpV4)
            return State::AddressFamilyNotMatch;

        SOCKADDR_IN serverAddr = GetSocketAddrIpV4(pSocket, endpoint);
        return ConnectInternal(pSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN), timeOutInMs);
    }

    template<>
    auto Connect<AddressFamily::IpV6>(const ResPtr<SocketHandle>& pSocket, const EndPointV6& endpoint, int timeOutInMs) -> State
    {
        if (pSocket == nullptr)
            return State::InvalidSocket;

        const auto socketFamily = GetSocketAddressFamily(pSocket);
        if (socketFamily != AddressFamily::IpV6)
            return State::AddressFamilyNotMatch;

        SOCKADDR_IN6 serverAddr = GetSocketAddrIpV6(pSocket, endpoint);
        return ConnectInternal(pSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN6), timeOutInMs);
    }

    template<>
    auto Bind<AddressFamily::IpV4>(const ResPtr<SocketHandle>& pSocket, const EndPointV4& endpoint) -> State
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

    template<>
    auto Bind<AddressFamily::IpV6>(const ResPtr<SocketHandle>& pSocket, const EndPointV6& endpoint) -> State
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

    auto Listen(const ResPtr<SocketHandle>& pSocket) -> State
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

    auto Accept(const ResPtr<SocketHandle>& pSocket, int timeOutInMs) -> std::pair<State, ResPtr<SocketHandle>>
    {
        if (pSocket == nullptr)
            return { State::InvalidSocket, nullptr };

        // Poll wait, timeout = -1 means infinity.
        WSAPOLLFD pollFd;
        pollFd.fd = pSocket->handle;
        pollFd.events = POLLRDNORM;

        int pollResult = ::WSAPoll(&pollFd, 1, timeOutInMs);
        if (pollResult == SOCKET_ERROR)
        {
            pSocket->lastError = ::WSAGetLastError();
            return { State::SocketError, nullptr };
        }

        if (pollResult == 0)
            return { State::Timeout, nullptr };

        // Check poll event
        if ((pollFd.revents & POLLRDNORM) == 0)
            return { State::ConnectionFailed, nullptr };

        const SOCKET clientSock = ::accept(pSocket->handle, nullptr, nullptr);
        if (clientSock == INVALID_SOCKET)
            return { State::ConnectionFailed, nullptr };

        return { State::Success, MakeSocketHandle(clientSock, GetSocketAddressFamily(pSocket), GetSocketProtocol(pSocket)) };
    }

    auto Send(const ResPtr<SocketHandle>& pSocket, const char* pDataBuffer, int bufferSize) -> State
    {
        if (pSocket == nullptr)
            return State::InvalidSocket;

        auto sendResult = ::send(pSocket->handle, pDataBuffer, bufferSize, 0);
        if (sendResult == SOCKET_ERROR)
        {
            pSocket->lastError = ::WSAGetLastError();
            return State::SocketError;
        }

        return State::Success;
    }

    auto Receive(const ResPtr<SocketHandle>& pSocket, char* pDataBuffer, int bufferSize, int timeOutInMs) -> std::pair<State, int>
    {
        if (pSocket == nullptr)
            return { State::InvalidSocket, 0 };

        // Poll wait, timeout = -1 means infinity.
        WSAPOLLFD pollFd;
        pollFd.fd = pSocket->handle;
        pollFd.events = POLLIN;

        int pollResult = ::WSAPoll(&pollFd, 1, timeOutInMs);
        if (pollResult == SOCKET_ERROR)
        {
            pSocket->lastError = ::WSAGetLastError();
            return { State::SocketError, 0 };
        }

        if (pollResult == 0)
            return { State::Timeout, 0 };

        // Check poll event
        if ((pollFd.revents & POLLIN) == 0)
            return { State::ReceiveFailed, 0 };

        const int bytesRead = ::recv(pSocket->handle, pDataBuffer, bufferSize, 0);
        if (bytesRead == SOCKET_ERROR)
            return { State::ReceiveFailed, 0 };

        if (bytesRead == 0)
            return { State::SocketClosed, 0 };

        return { State::Success, bytesRead };
    }
}

#endif