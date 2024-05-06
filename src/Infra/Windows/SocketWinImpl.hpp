#pragma once

#include "../PlatformDefine.hpp"

#if PLATFORM_WINDOWS

#include <mutex>
#include <thread>
#include <optional>
#include "../ResourcePtr.hpp"
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

    // Query
    inline auto GetSocketAddressFamily(const ResPtr<SocketHandle>& pSocket) -> AddressFamily
    {
        if (pSocket == nullptr)
            return AddressFamily::IpV4;

        return pSocket->family;
    }

    inline auto GetSocketProtocol(const ResPtr<SocketHandle>& pSocket) -> Protocol
    {
        if (pSocket == nullptr)
            return Protocol::Tcp;

        return pSocket->protocol;
    }

    inline auto GetSocketLastError() -> int
    {
        return ::WSAGetLastError();
    }

    namespace Impl
    {
        static bool gSocketEnvironmentInit = false;
        static std::mutex gInitLock {};

        inline ResPtr<SocketHandle> MakeSocketHandle(SOCKET handle, AddressFamily family, Protocol protocol)
        {
            ResPtr<SocketHandle> result(new SocketHandle());
            result->handle = handle;
            result->family = family;
            result->protocol = protocol;
            return result;
        }

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

        inline SOCKADDR_IN GetSocketAddrIpV4(const ResPtr<SocketHandle>& pSocket, const EndPointV4& endpoint)
        {
            SOCKADDR_IN serverAddr {};
            serverAddr.sin_family = GetWsaAddressFamily(Socket::GetSocketAddressFamily(pSocket));
            serverAddr.sin_port = ::htons(endpoint.GetPort());
            serverAddr.sin_addr.S_un.S_addr = ::htonl(endpoint.GetIp());
            return serverAddr;
        }

        inline SOCKADDR_IN6 GetSocketAddrIpV6(const ResPtr<Socket::SocketHandle>& pSocket, const EndPointV6& endpoint)
        {
            SOCKADDR_IN6 serverAddr {};
            serverAddr.sin6_family = GetWsaAddressFamily(Socket::GetSocketAddressFamily(pSocket));
            ::memcpy(&serverAddr.sin6_addr, endpoint.GetIp().data(), EndPointV6::ADDR_SIZE);
            serverAddr.sin6_scope_id = endpoint.GetScopeId();
            serverAddr.sin6_port = endpoint.GetPort();
            return serverAddr;
        }

        inline bool ConnectInternal(const ResPtr<Socket::SocketHandle>& pSocket, const SOCKADDR* pAddr, int size, int timeOutInMs)
        {
            const auto connectResult = ::connect(pSocket->handle, pAddr , size);

            // If not socket error, means connect success immediately, no need to poll.
            if (connectResult != SOCKET_ERROR)
                return true;

            // If last error not BLOCK or INPROGRESS, means an error really happened.
            const int lastError = ::WSAGetLastError();
            if (lastError != WSAEINPROGRESS && lastError != WSAEWOULDBLOCK)
                return false;

            // Poll wait, timeout = -1 means infinity.
            WSAPOLLFD pollFd;
            pollFd.fd = pSocket->handle;
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

        inline auto Create(AddressFamily family, Protocol protocol) -> ResPtr<SocketHandle>
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

        inline auto Destroy(ResPtr<SocketHandle>&& pSocket) -> void
        {
            if (pSocket == nullptr)
                return;

            ::closesocket(pSocket->handle);
        }

        template<AddressFamily addrFamily>
        inline auto Connect(const ResPtr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint, int timeOutInMs = -1) -> bool
        {
            if constexpr (addrFamily == AddressFamily::IpV4)
            {
                if (pSocket == nullptr)
                    return false;

                const auto socketFamily = GetSocketAddressFamily(pSocket);
                if (socketFamily != AddressFamily::IpV4)
                    return false;

                SOCKADDR_IN serverAddr = GetSocketAddrIpV4(pSocket, endpoint);
                return ConnectInternal(pSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN), timeOutInMs);
            }
            else if constexpr (addrFamily == AddressFamily::IpV6)
            {
                if (pSocket == nullptr)
                    return false;

                const auto socketFamily = GetSocketAddressFamily(pSocket);
                if (socketFamily != AddressFamily::IpV6)
                    return false;

                SOCKADDR_IN6 serverAddr = GetSocketAddrIpV6(pSocket, endpoint);
                return ConnectInternal(pSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN6), timeOutInMs);
            }
            else
            {
                ASSERT_MSG(false, "AddressFamily Error");
                return false;
            }
        }

        template<AddressFamily addrFamily>
        inline auto Bind(const ResPtr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint) -> bool
        {
            if constexpr (addrFamily == AddressFamily::IpV4)
            {
                if (pSocket == nullptr)
                    return false;

                const auto socketFamily = GetSocketAddressFamily(pSocket);
                if (socketFamily != AddressFamily::IpV4)
                    return false;

                SOCKADDR_IN serverAddr = GetSocketAddrIpV4(pSocket, endpoint);
                const auto bindResult = ::bind(pSocket->handle, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN));
                if (bindResult == SOCKET_ERROR)
                    return false;

                return true;
            }
            else if constexpr (addrFamily == AddressFamily::IpV6)
            {
                if (pSocket == nullptr)
                    return false;

                const auto socketFamily = GetSocketAddressFamily(pSocket);
                if (socketFamily != AddressFamily::IpV6)
                    return false;

                SOCKADDR_IN6 serverAddr = GetSocketAddrIpV6(pSocket, endpoint);
                const auto bindResult = ::bind(pSocket->handle, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN6));
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

        inline auto Listen(const ResPtr<SocketHandle>& pSocket) -> bool
        {
            if (pSocket == nullptr)
                return false;

            const auto result = ::listen(pSocket->handle, SOMAXCONN);
            if (result == SOCKET_ERROR)
                return false;

            return true;
        }

        inline auto Accept(const ResPtr<SocketHandle>& pSocket, int timeOutInMs = -1) -> std::optional<ResPtr<SocketHandle>>
        {
            if (pSocket == nullptr)
                return std::nullopt;

            // Poll wait, timeout = -1 means infinity.
            WSAPOLLFD pollFd;
            pollFd.fd = pSocket->handle;
            pollFd.events = POLLRDNORM;

            int pollResult = ::WSAPoll(&pollFd, 1, timeOutInMs);
            if (pollResult == SOCKET_ERROR)
                return std::nullopt;

            if (pollResult == 0)
                return std::nullopt;

            // Check poll event
            if ((pollFd.revents & POLLRDNORM) == 0)
                return std::nullopt;

            const SOCKET clientSock = ::accept(pSocket->handle, nullptr, nullptr);
            if (clientSock == INVALID_SOCKET)
                return std::nullopt;

            return MakeSocketHandle(clientSock, GetSocketAddressFamily(pSocket), GetSocketProtocol(pSocket));
        }

        inline auto Send(const ResPtr<SocketHandle>& pSocket, const char* pDataBuffer, int bufferSize) -> bool
        {
            if (pSocket == nullptr)
                return false;

            auto sendResult = ::send(pSocket->handle, pDataBuffer, bufferSize, 0);
            if (sendResult == SOCKET_ERROR)
                return false;

            return true;
        }

        inline auto Receive(const ResPtr<SocketHandle>& pSocket, char* pDataBuffer, int bufferSize, int timeOutInMs = -1) -> std::optional<int>
        {
            if (pSocket == nullptr)
                return std::nullopt;

            // Poll wait, timeout = -1 means infinity.
            WSAPOLLFD pollFd;
            pollFd.fd = pSocket->handle;
            pollFd.events = POLLIN;

            int pollResult = ::WSAPoll(&pollFd, 1, timeOutInMs);
            if (pollResult == SOCKET_ERROR)
                return std::nullopt;

            if (pollResult == 0)
                return std::nullopt; // timeout

            // Check poll event
            if ((pollFd.revents & POLLIN) == 0)
                return std::nullopt;

            const int bytesRead = ::recv(pSocket->handle, pDataBuffer, bufferSize, 0);
            if (bytesRead == SOCKET_ERROR)
                return std::nullopt;

            if (bytesRead == 0)
                return std::nullopt;

            return bytesRead;
        }
    }
}

#endif