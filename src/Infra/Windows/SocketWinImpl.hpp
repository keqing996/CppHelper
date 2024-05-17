#pragma once

#include "../PlatformDefine.hpp"

#if PLATFORM_WINDOWS

#include <mutex>
#include <thread>
#include <optional>
#include "../SocketBase.hpp"

#include <WinSock2.h>
#include <ws2ipdef.h>

#pragma comment(lib, "Ws2_32.lib")

namespace Infra::Socket
{
    struct SocketHandle
    {
        SOCKET handle;
        AddressFamily family;
        Protocol protocol;
    };

    inline auto GetSocketLastError() -> int
    {
        return ::WSAGetLastError();
    }

    namespace Impl
    {
        static bool gSocketEnvironmentInit = false;
        static std::mutex gInitLock {};

        inline int GetWsaAddressFamily(AddressFamily family)
        {
            switch (family)
            {
                case AddressFamily::IpV4:
                    return AF_INET;
                case AddressFamily::IpV6:
                    return AF_INET6;
            }
        }

        inline std::pair<int, int> GetWsaProtocol(Protocol protocol)
        {
            switch (protocol)
            {
                case Protocol::Tcp:
                    return std::make_pair(IPPROTO_TCP, SOCK_STREAM);
                case Protocol::Udp:
                    return std::make_pair(IPPROTO_UDP, SOCK_DGRAM);
            }
        }

        inline SOCKADDR_IN GetSocketAddrIpV4(const SocketHandle& socketHandle, const EndPointV4& endpoint)
        {
            SOCKADDR_IN serverAddr {};
            serverAddr.sin_family = GetWsaAddressFamily(socketHandle.family);
            serverAddr.sin_port = ::htons(endpoint.GetPort());
            serverAddr.sin_addr.S_un.S_addr = ::htonl(endpoint.GetIp());
            return serverAddr;
        }

        inline SOCKADDR_IN6 GetSocketAddrIpV6(const SocketHandle& socketHandle, const EndPointV6& endpoint)
        {
            SOCKADDR_IN6 serverAddr {};
            serverAddr.sin6_family = GetWsaAddressFamily(socketHandle.family);
            ::memcpy(&serverAddr.sin6_addr, endpoint.GetIp().data(), EndPointV6::ADDR_SIZE);
            serverAddr.sin6_scope_id = endpoint.GetScopeId();
            serverAddr.sin6_port = endpoint.GetPort();
            return serverAddr;
        }

        inline bool ConnectInternal(const SocketHandle& socketHandle, const SOCKADDR* pAddr, int size, int timeOutInMs)
        {
            const auto connectResult = ::connect(socketHandle.handle, pAddr , size);

            // If not socket error, means connect success immediately, no need to poll.
            if (connectResult != SOCKET_ERROR)
                return true;

            // If last error not BLOCK or INPROGRESS, means an error really happened.
            const int lastError = ::WSAGetLastError();
            if (lastError != WSAEINPROGRESS && lastError != WSAEWOULDBLOCK)
                return false;

            // Poll wait, timeout = -1 means infinity.
            WSAPOLLFD pollFd;
            pollFd.fd = socketHandle.handle;
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

        inline auto IsInitialized() -> bool
        {
            return gSocketEnvironmentInit;
        }

        inline auto InitEnvironment() -> bool
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

        inline auto DestroyEnvironment() -> void
        {
            std::lock_guard<std::mutex> lock(gInitLock);
            {
                ::WSACleanup();
                gSocketEnvironmentInit = false;
            }
        }

        inline auto Create(AddressFamily family, Protocol protocol) -> std::optional<SocketHandle>
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

            return SocketHandle { socket, family, protocol };
        }

        inline auto Destroy(const SocketHandle& socketHandle) -> void
        {
            ::closesocket(socketHandle.handle);
        }

        template<AddressFamily addrFamily>
        inline auto Connect(const SocketHandle& socketHandle, const EndPoint<addrFamily>& endpoint, int timeOutInMs = -1) -> bool
        {
            if constexpr (addrFamily == AddressFamily::IpV4)
            {
                if (socketHandle.family != AddressFamily::IpV4)
                    return false;

                SOCKADDR_IN serverAddr = GetSocketAddrIpV4(socketHandle, endpoint);
                return ConnectInternal(socketHandle, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN), timeOutInMs);
            }
            else if constexpr (addrFamily == AddressFamily::IpV6)
            {
                if (socketHandle.family != AddressFamily::IpV6)
                    return false;

                SOCKADDR_IN6 serverAddr = GetSocketAddrIpV6(socketHandle, endpoint);
                return ConnectInternal(socketHandle, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN6), timeOutInMs);
            }
            else
            {
                ASSERT_MSG(false, "AddressFamily Error");
                return false;
            }
        }

        template<AddressFamily addrFamily>
        inline auto Bind(const SocketHandle& socketHandle, const EndPoint<addrFamily>& endpoint) -> bool
        {
            if constexpr (addrFamily == AddressFamily::IpV4)
            {
                if (socketHandle.family != AddressFamily::IpV4)
                    return false;

                SOCKADDR_IN serverAddr = GetSocketAddrIpV4(socketHandle, endpoint);
                const auto bindResult = ::bind(socketHandle.handle, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN));
                if (bindResult == SOCKET_ERROR)
                    return false;

                return true;
            }
            else if constexpr (addrFamily == AddressFamily::IpV6)
            {
                if (socketHandle.family != AddressFamily::IpV6)
                    return false;

                SOCKADDR_IN6 serverAddr = GetSocketAddrIpV6(socketHandle, endpoint);
                const auto bindResult = ::bind(socketHandle.handle, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN6));
                if (bindResult == SOCKET_ERROR)
                    return false;

                return true;
            }
            else
            {
                ASSERT_MSG(false, "AddressFamily Error");
                return false;
            }
        }

        inline auto Listen(const SocketHandle& pSocket) -> bool
        {
            const auto result = ::listen(pSocket.handle, SOMAXCONN);
            if (result == SOCKET_ERROR)
                return false;

            return true;
        }

        inline auto Accept(const SocketHandle& socketHandle, int timeOutInMs = -1) -> std::optional<SocketHandle>
        {
            // Poll wait, timeout = -1 means infinity.
            WSAPOLLFD pollFd;
            pollFd.fd = socketHandle.handle;
            pollFd.events = POLLRDNORM;

            int pollResult = ::WSAPoll(&pollFd, 1, timeOutInMs);
            if (pollResult == SOCKET_ERROR)
                return std::nullopt;

            if (pollResult == 0)
                return std::nullopt;

            // Check poll event
            if ((pollFd.revents & POLLRDNORM) == 0)
                return std::nullopt;

            const SOCKET clientSock = ::accept(socketHandle.handle, nullptr, nullptr);
            if (clientSock == INVALID_SOCKET)
                return std::nullopt;

            return SocketHandle {clientSock, socketHandle.family, socketHandle.protocol };
        }

        inline auto Send(const SocketHandle& socketHandle, const char* pDataBuffer, int bufferSize) -> bool
        {
            auto sendResult = ::send(socketHandle.handle, pDataBuffer, bufferSize, 0);
            if (sendResult == SOCKET_ERROR)
                return false;

            return true;
        }

        inline auto Receive(const SocketHandle& socketHandle, char* pDataBuffer, int bufferSize, int timeOutInMs = -1) -> std::optional<int>
        {
            // Poll wait, timeout = -1 means infinity.
            WSAPOLLFD pollFd;
            pollFd.fd = socketHandle.handle;
            pollFd.events = POLLIN;

            int pollResult = ::WSAPoll(&pollFd, 1, timeOutInMs);
            if (pollResult == SOCKET_ERROR)
                return std::nullopt;

            if (pollResult == 0)
                return std::nullopt; // timeout

            // Check poll event
            if ((pollFd.revents & POLLIN) == 0)
                return std::nullopt;

            const int bytesRead = ::recv(socketHandle.handle, pDataBuffer, bufferSize, 0);
            if (bytesRead == SOCKET_ERROR)
                return std::nullopt;

            if (bytesRead == 0)
                return std::nullopt;

            return bytesRead;
        }
    }
}

#endif