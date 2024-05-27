#include "Infra/Windows/WindowsDefine.hpp"

#if PLATFORM_WINDOWS

#include <mutex>
#include <thread>
#include <optional>
#include "Infra/Socket.hpp"
#include "Infra/String.hpp"

#include <WinSock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

static bool gSocketEnvironmentInit = false;
static std::mutex gInitLock{};

static int GetWsaAddressFamily(Infra::Socket::AddressFamily family)
{
    switch (family)
    {
        case Infra::Socket::AddressFamily::IpV4:
            return AF_INET;
        case Infra::Socket::AddressFamily::IpV6:
            return AF_INET6;
    }
}

static std::pair<int, int> GetWsaProtocol(Infra::Socket::Protocol protocol)
{
    switch (protocol)
    {
        case Infra::Socket::Protocol::Tcp:
            return std::make_pair(IPPROTO_TCP, SOCK_STREAM);
        case Infra::Socket::Protocol::Udp:
            return std::make_pair(IPPROTO_UDP, SOCK_DGRAM);
    }
}

static SOCKADDR_IN GetSocketAddrIpV4(const Infra::Socket::SocketHandle& socketHandle, const Infra::Socket::EndPointV4& endpoint)
{
    SOCKADDR_IN serverAddr{};
    serverAddr.sin_family = GetWsaAddressFamily(socketHandle.family);
    serverAddr.sin_port = ::htons(endpoint.GetPort());
    serverAddr.sin_addr.S_un.S_addr = ::htonl(endpoint.GetIp());
    return serverAddr;
}

static SOCKADDR_IN6 GetSocketAddrIpV6(const Infra::Socket::SocketHandle& socketHandle,
                                      const Infra::Socket::EndPointV6& endpoint)
{
    SOCKADDR_IN6 serverAddr{};
    serverAddr.sin6_family = GetWsaAddressFamily(socketHandle.family);
    ::memcpy(&serverAddr.sin6_addr, endpoint.GetIp().data(), Infra::Socket::EndPointV6::ADDR_SIZE);
    serverAddr.sin6_scope_id = endpoint.GetScopeId();
    serverAddr.sin6_port = endpoint.GetPort();
    return serverAddr;
}

static bool ConnectInternal(const Infra::Socket::SocketHandle& socketHandle, const SOCKADDR* pAddr, int size, int timeOutInMs)
{
    SOCKET socket = reinterpret_cast<SOCKET>(socketHandle.handle);
    const auto connectResult = ::connect(socket, pAddr, size);

    // If not socket error, means connect success immediately, no need to poll.
    if (connectResult != SOCKET_ERROR)
        return true;

    // If last error not BLOCK or INPROGRESS, means an error really happened.
    const int lastError = ::WSAGetLastError();
    if (lastError != WSAEINPROGRESS && lastError != WSAEWOULDBLOCK)
        return false;

    // Poll wait, timeout = -1 means infinity.
    WSAPOLLFD pollFd;
    pollFd.fd = socket;
    pollFd.events = POLLOUT;

    int pollResult = ::WSAPoll(&pollFd, 1, timeOutInMs);
    if (pollResult == SOCKET_ERROR)
        return false;

    if (pollResult == 0)
        return false;

    // Check poll event
    if ((pollFd.revents & POLLOUT) == 0)
        return false;

    return true;
}

namespace Infra
{
    Socket::EndPoint<Socket::AddressFamily::IpV4>::EndPoint(uint32_t ip, uint16_t port): _ip(ip)
        , _port(port)
    {
    }

    uint32_t Socket::EndPoint<Socket::AddressFamily::IpV4>::GetIp() const
    {
        return _ip;
    }

    uint16_t Socket::EndPoint<Socket::AddressFamily::IpV4>::GetPort() const
    {
        return _port;
    }

    std::optional<Socket::EndPoint<Socket::AddressFamily::IpV4>> Socket::EndPoint<Socket::AddressFamily::IpV4>::TryCreate(const std::string& ip, uint16_t port)
    {
        std::wstring ipInWide = String::StringToWideString(ip);

        uint32_t ip;
        int result = ::InetPtonW(AF_INET, ipInWide.c_str(), &ip);
        if (SUCCEEDED(result))
            return EndPoint(ip, port);

        return std::nullopt;
    }

    Socket::EndPoint<
        Socket::AddressFamily::IpV6>::EndPoint(const uint8_t* array, uint16_t port, uint32_t scopeId): _port(port)
        , _scopeId(scopeId)
    {
        ::memcpy(_ip.data(), array, ADDR_SIZE);
    }

    const std::array<uint8_t, Socket::EndPoint<Socket::AddressFamily::IpV6>::ADDR_SIZE>& Socket::EndPoint<Socket::
        AddressFamily::IpV6>::GetIp() const
    {
        return _ip;
    }

    uint16_t Socket::EndPoint<Socket::AddressFamily::IpV6>::GetPort() const
    {
        return _port;
    }

    uint32_t Socket::EndPoint<Socket::AddressFamily::IpV6>::GetScopeId() const
    {
        return _scopeId;
    }

    auto Socket::IsInitialized() -> bool
    {
        return gSocketEnvironmentInit;
    }

    auto Socket::InitEnvironment() -> bool
    {
        std::lock_guard<std::mutex> lock(gInitLock);
        {
            WORD wVersion = MAKEWORD(2, 2);
            WSADATA wsadata;

            const auto wsaStartUpResult = ::WSAStartup(wVersion, &wsadata);
            if (wsaStartUpResult != 0)
                return false;

            if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2)
                return false;

            gSocketEnvironmentInit = true;

            return true;
        }
    }

    auto Socket::DestroyEnvironment() -> void
    {
        std::lock_guard<std::mutex> lock(gInitLock);
        {
            ::WSACleanup();
            gSocketEnvironmentInit = false;
        }
    }

    auto Socket::Create(AddressFamily family, Protocol protocol) -> std::optional<SocketHandle>
    {
        const int wsaAddrFamily = GetWsaAddressFamily(family);
        const auto [wsaProtocol, wsaSocketType] = GetWsaProtocol(protocol);

        const SOCKET socket = ::socket(wsaAddrFamily, wsaSocketType, wsaProtocol);
        if (socket == INVALID_SOCKET)
            return std::nullopt;

        // Set socket non-blocking
        u_long mode = 1;
        const int setAsyncResult = ioctlsocket(socket, FIONBIO, &mode);
        if (setAsyncResult != 0)
        {
            ::closesocket(socket);
            return std::nullopt;
        }

        return SocketHandle{ reinterpret_cast<void*>(socket), family, protocol };
    }

    auto Socket::Destroy(const SocketHandle& socketHandle) -> void
    {
        SOCKET socket = reinterpret_cast<SOCKET>(socketHandle.handle);
        ::closesocket(socket);
    }

    template<Socket::AddressFamily addrFamily>
    auto Socket::Connect(const SocketHandle& socketHandle, const EndPoint<addrFamily>& endpoint, int timeOutInMs) -> bool
    {
        if constexpr (addrFamily == AddressFamily::IpV4)
        {
            if (socketHandle.family != AddressFamily::IpV4)
                return false;

            SOCKADDR_IN serverAddr = GetSocketAddrIpV4(socketHandle, endpoint);
            return ConnectInternal(socketHandle, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN),
                                   timeOutInMs);
        } else if constexpr (addrFamily == AddressFamily::IpV6)
        {
            if (socketHandle.family != AddressFamily::IpV6)
                return false;

            SOCKADDR_IN6 serverAddr = GetSocketAddrIpV6(socketHandle, endpoint);
            return ConnectInternal(socketHandle, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN6),
                                   timeOutInMs);
        } else
        {
            ASSERT_MSG(false, "AddressFamily Error");
            return false;
        }
    }

    template<Socket::AddressFamily addrFamily>
    auto Socket::Bind(const SocketHandle& socketHandle, const EndPoint<addrFamily>& endpoint) -> bool
    {
        SOCKET socket = reinterpret_cast<SOCKET>(socketHandle.handle);
        if constexpr (addrFamily == AddressFamily::IpV4)
        {
            if (socketHandle.family != AddressFamily::IpV4)
                return false;

            SOCKADDR_IN serverAddr = GetSocketAddrIpV4(socketHandle, endpoint);
            const auto bindResult = ::bind(socket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN));
            if (bindResult == SOCKET_ERROR)
                return false;

            return true;
        } else if constexpr (addrFamily == AddressFamily::IpV6)
        {
            if (socketHandle.family != AddressFamily::IpV6)
                return false;

            SOCKADDR_IN6 serverAddr = GetSocketAddrIpV6(socketHandle, endpoint);
            const auto bindResult = ::bind(socket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN6));
            if (bindResult == SOCKET_ERROR)
                return false;

            return true;
        } else
        {
            ASSERT_MSG(false, "AddressFamily Error");
            return false;
        }
    }

    auto Socket::Listen(const SocketHandle& socketHandle) -> bool
    {
        SOCKET socket = reinterpret_cast<SOCKET>(socketHandle.handle);
        const auto result = ::listen(socket, SOMAXCONN);
        if (result == SOCKET_ERROR)
            return false;

        return true;
    }

    auto Socket::Accept(const SocketHandle& socketHandle, int timeOutInMs) -> std::optional<SocketHandle>
    {
        SOCKET socket = reinterpret_cast<SOCKET>(socketHandle.handle);

        // Poll wait, timeout = -1 means infinity.
        WSAPOLLFD pollFd;
        pollFd.fd = socket;
        pollFd.events = POLLRDNORM;

        int pollResult = ::WSAPoll(&pollFd, 1, timeOutInMs);
        if (pollResult == SOCKET_ERROR)
            return std::nullopt;

        if (pollResult == 0)
            return std::nullopt;

        // Check poll event
        if ((pollFd.revents & POLLRDNORM) == 0)
            return std::nullopt;

        const SOCKET clientSock = ::accept(socket, nullptr, nullptr);
        if (clientSock == INVALID_SOCKET)
            return std::nullopt;

        return SocketHandle{ reinterpret_cast<void*>(clientSock), socketHandle.family, socketHandle.protocol };
    }

    auto Socket::Send(const SocketHandle& socketHandle, const char* pDataBuffer, int bufferSize) -> bool
    {
        SOCKET socket = reinterpret_cast<SOCKET>(socketHandle.handle);
        auto sendResult = ::send(socket, pDataBuffer, bufferSize, 0);
        if (sendResult == SOCKET_ERROR)
            return false;

        return true;
    }

    auto Socket::Receive(const SocketHandle& socketHandle, char* pDataBuffer, int bufferSize, int timeOutInMs) -> std::optional<int>
    {
        SOCKET socket = reinterpret_cast<SOCKET>(socketHandle.handle);

        // Poll wait, timeout = -1 means infinity.
        WSAPOLLFD pollFd;
        pollFd.fd = socket;
        pollFd.events = POLLIN;

        int pollResult = ::WSAPoll(&pollFd, 1, timeOutInMs);
        if (pollResult == SOCKET_ERROR)
            return std::nullopt;

        if (pollResult == 0)
            return std::nullopt; // timeout

        // Check poll event
        if ((pollFd.revents & POLLIN) == 0)
            return std::nullopt;

        const int bytesRead = ::recv(socket, pDataBuffer, bufferSize, 0);
        if (bytesRead == SOCKET_ERROR)
            return std::nullopt;

        if (bytesRead == 0)
            return std::nullopt;

        return bytesRead;
    }
}

#endif
