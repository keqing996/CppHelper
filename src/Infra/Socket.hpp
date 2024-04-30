#pragma once

#include <memory>
#include <array>

#include "PlatformDefine.hpp"
#include "ResourcePtr.hpp"
#include "ScopeGuard.hpp"

#if PLATFORM_WINDOWS
#   include <WinSock2.h>
#   include <ws2ipdef.h>
#   pragma comment(lib, "Ws2_32.lib")
#endif

namespace Infra
{
    class Socket
    {
    public:
        Socket() = delete;

    public:
        enum class State
        {
            Success,
            Timeout,
            SystemError,
            SocketError,
            WsaVersionError,
            InvalidSocket,
            AddressFamilyNotMatch,
            ConnectionFailed,
            AcceptFailed,
            ReceiveFailed,
            SocketClosed,
        };

        enum class AddressFamily
        {
            IpV4,
            IpV6
        };

        enum class Protocol
        {
            Tcp,
            Udp
        };

        template<AddressFamily>
        class EndPoint
        {
        };

        template<>
        class EndPoint<AddressFamily::IpV4>
        {
        public:
            EndPoint(uint32_t ip, uint16_t port) : _ip(ip), _port(port) {}

        public:
            auto GetIp() const -> uint32_t { return _ip; }
            auto GetPort() const -> uint16_t { return _port; }

        private:
            uint32_t _ip;
            uint16_t _port;
        };

        template<>
        class EndPoint<AddressFamily::IpV6>
        {
        public:
            static int constexpr ADDR_SIZE = 16;

        public:
            EndPoint(const uint8_t* array, uint16_t port, uint32_t scopeId)
                    : _port(port), _scopeId(scopeId)
            {
                ::memcpy(_ip.data(), array, ADDR_SIZE);
            }

        public:
            auto GetIp() const -> const std::array<uint8_t, ADDR_SIZE>& { return _ip; }
            auto GetPort() const -> uint16_t { return _port; }
            auto GetScopeId() const -> uint32_t { return _scopeId; }

        private:
            std::array<uint8_t, ADDR_SIZE> _ip{};
            uint16_t _port;
            uint32_t _scopeId;
        };

        using EndPointV4 = EndPoint<AddressFamily::IpV4>;
        using EndPointV6 = EndPoint<AddressFamily::IpV6>;

        struct SocketHandle;

    public:
        // Init and clean
        static auto IsInitialized() -> bool;
        static auto InitEnvironment() -> State;
        static auto DestroyEnvironment() -> void;

        // Create and destroy
        static auto Create(AddressFamily family, Protocol protocol) -> ResPtr<SocketHandle>;
        static auto Destroy(ResPtr<SocketHandle>&& pSocket) -> void;

        // Query
        static auto GetSocketAddressFamily(const ResPtr<SocketHandle>& pSocket) -> AddressFamily;
        static auto GetSocketProtocol(const ResPtr<SocketHandle>& pSocket) -> Protocol;

        // Error handle
        static auto GetSystemLastError() -> int;
        static auto GetSocketLastError(const ResPtr<SocketHandle>& pSocket) -> int;

        // Client
        template<AddressFamily addrFamily>
        static auto Connect(const ResPtr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint, int timeOutInMs = -1) -> State;

        // Server
        template<AddressFamily addrFamily>
        static auto Bind(const ResPtr<SocketHandle>& pSocket, const EndPoint<addrFamily>& endpoint) -> State;

        static auto Listen(const ResPtr<SocketHandle>& pSocket) -> State;
        static auto Accept(const ResPtr<SocketHandle>& pSocket, int timeOutInMs = -1) -> std::pair<State, ResPtr<SocketHandle>>;

        // Send & Recv
        static auto Send(const ResPtr<SocketHandle>& pSocket, const char* pDataBuffer, int bufferSize) -> State;
        static auto Receive(const ResPtr<SocketHandle>& pSocket, char* pDataBuffer, int bufferSize, int timeOutInMs = -1) -> std::pair<State, int>;
    };

    template<Socket::AddressFamily addrFamily>
    auto
    Socket::Bind(const ResPtr<SocketHandle>& pSocket, const Socket::EndPoint<addrFamily>& endpoint) -> Socket::State
    {
        ASSERT_MSG(false, "AddressFamily Error");
        return State::SystemError;
    }

    template<Socket::AddressFamily addrFamily>
    auto Socket::Connect(const ResPtr<SocketHandle>& pSocket, const Socket::EndPoint<addrFamily>& endpoint,
                         int timeOutInMs) -> Socket::State
    {
        ASSERT_MSG(false, "AddressFamily Error");
        return State::SystemError;
    }

#pragma region [Win Wsa Socket]

#if PLATFORM_WINDOWS

    struct Socket::SocketHandle
    {
        SOCKET handle;
        AddressFamily family;
        Protocol protocol;
        int lastError;
    };

    class SocketWsaSupport
    {
    public:
        SocketWsaSupport() = delete;

    public:
        inline static bool gSocketEnvironmentInit = false;

    public:
        static ResPtr<Socket::SocketHandle> MakeSocketHandle(SOCKET handle, Socket::AddressFamily family, Socket::Protocol protocol)
        {
            ResPtr<Socket::SocketHandle> result(new Socket::SocketHandle());
            result->handle = handle;
            result->family = family;
            result->protocol = protocol;
            result->lastError = 0;
            return result;
        }

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

        static SOCKADDR_IN GetSocketAddrIpV4(const ResPtr<Socket::SocketHandle>& pSocket, const Socket::EndPointV4& endpoint)
        {
            SOCKADDR_IN serverAddr {};
            serverAddr.sin_family = GetWsaAddressFamily(Socket::GetSocketAddressFamily(pSocket));
            serverAddr.sin_port = ::htons(endpoint.GetPort());
            serverAddr.sin_addr.S_un.S_addr = ::htonl(endpoint.GetIp());
            return serverAddr;
        }

        static SOCKADDR_IN6 GetSocketAddrIpV6(const ResPtr<Socket::SocketHandle>& pSocket, const Socket::EndPointV6& endpoint)
        {
            SOCKADDR_IN6 serverAddr {};
            serverAddr.sin6_family = GetWsaAddressFamily(Socket::GetSocketAddressFamily(pSocket));
            ::memcpy(&serverAddr.sin6_addr, endpoint.GetIp().data(), Socket::EndPointV6::ADDR_SIZE);
            serverAddr.sin6_scope_id = endpoint.GetScopeId();
            serverAddr.sin6_port = endpoint.GetPort();
            return serverAddr;
        }

        static Socket::State ConnectInternal(const ResPtr<Socket::SocketHandle>& pSocket, const SOCKADDR* pAddr, int size, int timeOutInMs)
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

            int pollResult = ::WSAPoll(&pollFd, 1, timeOutInMs);
            if (pollResult == SOCKET_ERROR)
            {
                pSocket->lastError = ::WSAGetLastError();
                return Socket::State::SocketError;
            }

            if (pollResult == 0)
                return Socket::State::Timeout;

            // Check poll event
            if ((pollFd.revents & POLLOUT) == 0)
                return Socket::State::ConnectionFailed;

            return Socket::State::Success;
        }
    };

    bool Socket::IsInitialized()
    {
        return SocketWsaSupport::gSocketEnvironmentInit;
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

        SocketWsaSupport::gSocketEnvironmentInit = true;
        return State::Success;
    }

    void Socket::DestroyEnvironment()
    {
        ::WSACleanup();
        SocketWsaSupport::gSocketEnvironmentInit = false;
    }

    ResPtr<Socket::SocketHandle> Socket::Create(AddressFamily family, Protocol protocol)
    {
        const int wsaAddrFamily = SocketWsaSupport::GetWsaAddressFamily(family);
        const auto [wsaProtocol, wsaSocketType] = SocketWsaSupport::GetWsaProtocol(protocol);

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

        return SocketWsaSupport::MakeSocketHandle(socket, family, protocol);
    }

    void Socket::Destroy(ResPtr<SocketHandle>&& pSocket)
    {
        if (pSocket == nullptr)
            return;

        ::closesocket(pSocket->handle);
    }

    auto Socket::GetSocketAddressFamily(const ResPtr<SocketHandle>& pSocket) -> AddressFamily
    {
        if (pSocket == nullptr)
            return AddressFamily::IpV4;

        return pSocket->family;
    }

    auto Socket::GetSocketProtocol(const ResPtr<SocketHandle>& pSocket) -> Protocol
    {
        if (pSocket == nullptr)
            return Protocol::Tcp;

        return pSocket->protocol;
    }

    auto Socket::GetSystemLastError() -> int
    {
        return ::WSAGetLastError();
    }

    auto Socket::GetSocketLastError(const ResPtr<SocketHandle>& pSocket) -> int
    {
        if (pSocket == nullptr)
            return 0;

        return pSocket->lastError;
    }

    template<>
    auto Socket::Connect<Socket::AddressFamily::IpV4>(const ResPtr<SocketHandle>& pSocket, const EndPointV4& endpoint, int timeOutInMs) -> State
    {
        if (pSocket == nullptr)
            return State::InvalidSocket;

        const auto socketFamily = GetSocketAddressFamily(pSocket);
        if (socketFamily != AddressFamily::IpV4)
            return State::AddressFamilyNotMatch;

        SOCKADDR_IN serverAddr = SocketWsaSupport::GetSocketAddrIpV4(pSocket, endpoint);
        return SocketWsaSupport::ConnectInternal(pSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN), timeOutInMs);
    }

    template<>
    auto Socket::Connect<Socket::AddressFamily::IpV6>(const ResPtr<SocketHandle>& pSocket, const EndPointV6& endpoint, int timeOutInMs) -> State
    {
        if (pSocket == nullptr)
            return State::InvalidSocket;

        const auto socketFamily = GetSocketAddressFamily(pSocket);
        if (socketFamily != AddressFamily::IpV6)
            return State::AddressFamilyNotMatch;

        SOCKADDR_IN6 serverAddr = SocketWsaSupport::GetSocketAddrIpV6(pSocket, endpoint);
        return SocketWsaSupport::ConnectInternal(pSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN6), timeOutInMs);
    }

    template<>
    auto Socket::Bind<Socket::AddressFamily::IpV4>(const ResPtr<SocketHandle>& pSocket, const EndPointV4& endpoint) -> State
    {
        if (pSocket == nullptr)
            return State::InvalidSocket;

        const auto socketFamily = GetSocketAddressFamily(pSocket);
        if (socketFamily != AddressFamily::IpV4)
            return State::AddressFamilyNotMatch;

        SOCKADDR_IN serverAddr = SocketWsaSupport::GetSocketAddrIpV4(pSocket, endpoint);
        const auto bindResult = ::bind(pSocket->handle, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN));
        if (bindResult == SOCKET_ERROR)
        {
            pSocket->lastError = ::WSAGetLastError();
            return State::SocketError;
        }

        return State::Success;
    }

    template<>
    auto Socket::Bind<Socket::AddressFamily::IpV6>(const ResPtr<SocketHandle>& pSocket, const EndPointV6& endpoint) -> State
    {
        if (pSocket == nullptr)
            return State::InvalidSocket;

        const auto socketFamily = GetSocketAddressFamily(pSocket);
        if (socketFamily != AddressFamily::IpV6)
            return State::AddressFamilyNotMatch;

        SOCKADDR_IN6 serverAddr = SocketWsaSupport::GetSocketAddrIpV6(pSocket, endpoint);
        const auto bindResult = ::bind(pSocket->handle, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(SOCKADDR_IN6));
        if (bindResult == SOCKET_ERROR)
        {
            pSocket->lastError = ::WSAGetLastError();
            return State::SocketError;
        }

        return State::Success;
    }

    auto Socket::Listen(const ResPtr<SocketHandle>& pSocket) -> State
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

    auto Socket::Accept(const ResPtr<SocketHandle>& pSocket, int timeOutInMs) -> std::pair<State, ResPtr<SocketHandle>>
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

        return { State::Success, SocketWsaSupport::MakeSocketHandle(clientSock, GetSocketAddressFamily(pSocket), GetSocketProtocol(pSocket)) };
    }

    auto Socket::Send(const ResPtr<SocketHandle>& pSocket, const char* pDataBuffer, int bufferSize) -> State
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

    auto Socket::Receive(const ResPtr<SocketHandle>& pSocket, char* pDataBuffer, int bufferSize, int timeOutInMs) -> std::pair<State, int>
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

#endif

#pragma endregion
}