#include "WindowsPlatform.h"
#include "WinApiSocket.h"
#include <WinSock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <format>

#pragma comment(lib, "Ws2_32.lib")

namespace Helper::Win::Socket
{

    static std::string gLastError {};

    EnumEventResult::EnumEventResult()
    {
        std::fill(_events.begin(), _events.end(), false);
    }

    bool EnumEventResult::operator[](EventType t) const
    {
        return _events[static_cast<int>(t)];
    }

    bool& EnumEventResult::operator[](EventType t)
    {
        return _events[static_cast<int>(t)];
    }

    template<typename T>
    inline void* TypeToHandle(T x)
    {
        return reinterpret_cast<void*>(x);
    }

    template<typename T>
    inline T HandleToType(void* x)
    {
        return reinterpret_cast<T>(x);
    }

    std::optional<SOCKADDR_IN> CreateAddrFromIpv4(const std::string& ipStr, int port)
    {
        in_addr dst{};
        int result = ::InetPtonA(AF_INET, ipStr.c_str(), &dst);
        if (result != 1)
        {
            gLastError = std::format("ip error: {}", ::WSAGetLastError());
            return std::nullopt;
        }

        SOCKADDR_IN sockAddrIn;
        sockAddrIn.sin_family = AF_INET;
        sockAddrIn.sin_addr.S_un.S_addr = dst.S_un.S_addr;
        sockAddrIn.sin_port = ::htons(port);

        return sockAddrIn;
    }

    std::optional<SocketAddr> GetAddrFromSocketAddr(const SOCKADDR_IN& addr)
    {
        char pIpStr[16] {0};
        auto pErrMsg = ::InetNtopA(AF_INET, &addr.sin_addr.S_un.S_addr, pIpStr, sizeof(pIpStr));

        if (pErrMsg != nullptr)
        {
            gLastError = std::string { pErrMsg };
            return std::nullopt;
        }

        auto port = ::ntohs(addr.sin_port);
        SocketAddr result { {pIpStr}, port };
        return result;
    }

    template<int FD, int FD_BIT>
    bool GetEnumEventsFdBitResult(const WSANETWORKEVENTS& wsaNetEvents)
    {
        return (wsaNetEvents.lNetworkEvents & FD)
               && (wsaNetEvents.iErrorCode[FD_BIT] == 0);
    }

    const std::string& LastErrorMessage()
    {
        return gLastError;
    }

    bool StartUp()
    {
        WORD wVersion = MAKEWORD(2, 2);
        WSADATA wsadata;

        auto wsaStartUpResult = ::WSAStartup(wVersion, &wsadata);
        if (0 != wsaStartUpResult)
        {
            gLastError = std::format("Wsa start up failed with code {}", wsaStartUpResult);
            return false;
        }

        if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2)
        {
            gLastError = std::format("Wsa version not 2.2, is {}.{}", wsadata.wHighVersion, wsadata.wVersion);
            return false;
        }

        return true;
    }

    void CleanUp()
    {
        ::WSACleanup();
    }

    std::optional<void*> CreateIpv4TcpSocket()
    {
        int addressFamily = AF_INET;
        int socketType = SOCK_STREAM;
        int protocol = IPPROTO_TCP;

        SOCKET socket = ::socket(addressFamily, socketType, protocol);

        if (socket == INVALID_SOCKET)
        {
            gLastError = std::format("socket create failed, af = {}, type = {}. protocol = {}",
                                     addressFamily, socketType, protocol);
            return std::nullopt;
        }

        return TypeToHandle(socket);
    }

    void CloseSocket(SocketHandle socket)
    {
        ::closesocket(HandleToType<SOCKET>(socket));
    }

    bool Send(SocketHandle socket, Byte* pDataBuffer, int bufferSize)
    {
        auto sendResult = ::send(HandleToType<SOCKET>(socket), pDataBuffer, bufferSize, 0);
        if (sendResult == SOCKET_ERROR)
        {
            gLastError = std::format("socket send error: {}", ::WSAGetLastError());
            return false;
        }

        return true;
    }

    std::optional<int> Receive(SocketHandle socket, Byte* pDataBuffer, int bufferSize)
    {
        // receiveResult < 0 -> error
        // receiveResult > 0 -> receive size
        auto receiveResult = ::recv(HandleToType<SOCKET>(socket), pDataBuffer, bufferSize, 0);
        if (receiveResult == SOCKET_ERROR)
        {
            gLastError = std::format("socket receive error: {}", ::WSAGetLastError());
            return std::nullopt;
        }

        return receiveResult;
    }

    bool Connect(SocketHandle socket, const std::string& ipStr, int port)
    {
        auto createAddrResult = CreateAddrFromIpv4(ipStr, port);
        if (!createAddrResult.has_value())
            return false;

        SOCKADDR_IN serverAddr = createAddrResult.value();

        auto connectResult = ::connect(
                HandleToType<SOCKET>(socket),
                HandleToType<SOCKADDR*>(&serverAddr),
                sizeof(SOCKADDR));

        if (connectResult == SOCKET_ERROR)
        {
            gLastError = std::format("socket connect error: {}", ::WSAGetLastError());
            return false;
        }

        return true;
    }

    bool Bind(SocketHandle socket, const std::string& ipStr, int port)
    {
        auto createAddrResult = CreateAddrFromIpv4(ipStr, port);
        if (!createAddrResult.has_value())
            return false;

        SOCKADDR_IN serverAddr = createAddrResult.value();

        auto bindResult = ::bind(
                HandleToType<SOCKET>(socket),
                HandleToType<SOCKADDR*>(&serverAddr),
                sizeof(SOCKADDR));

        if (bindResult == SOCKET_ERROR)
        {
            gLastError = std::format("socket bind error: {}", ::WSAGetLastError());
            return false;
        }

        return true;
    }

    bool Listen(SocketHandle socket)
    {
        auto bindResult = ::listen(HandleToType<SOCKET>(socket), SOMAXCONN);
        if (bindResult == SOCKET_ERROR)
        {
            gLastError = std::format("socket listen error: {}", ::WSAGetLastError());
            return false;
        }

        return true;
    }

    std::optional<SocketAddr> Accept(SocketHandle socket)
    {
        sockaddr_in addrIn {};
        int len = sizeof(sockaddr_in);
        auto acceptResult = ::accept(HandleToType<SOCKET>(socket), (sockaddr*)&addrIn, &len);
        if (acceptResult == SOCKET_ERROR)
        {
            gLastError = std::format("socket accept error: {}", ::WSAGetLastError());
            return std::nullopt;
        }

        return GetAddrFromSocketAddr(addrIn);
    }

    std::optional<WsaEventHandle> CreateWsaEvent()
    {
        WSAEVENT wsaEvent = ::WSACreateEvent();
        if (wsaEvent == WSA_INVALID_EVENT)
        {
            gLastError = std::format("socket create event error: {}", ::WSAGetLastError());
            return std::nullopt;
        }

        return TypeToHandle(wsaEvent);
    }

    void CloseWsaEvent(WsaEventHandle wsaEvent)
    {
        ::WSACloseEvent(wsaEvent);
    }

    bool WsaEventSelect(SocketHandle socket, WsaEventHandle wsaEvent, const EventType* events, uint32_t eventNum)
    {
        long mask = 0;
        for (int i = 0; i < eventNum; i++)
        {
            switch (events[i])
            {
                case EventType::Read:   mask |= FD_READ; break;
                case EventType::Write:  mask |= FD_WRITE; break;
                case EventType::Accept: mask |= FD_ACCEPT; break;
                case EventType::Connect:mask |= FD_CONNECT; break;
                case EventType::Close:  mask |= FD_CLOSE; break;
                case EventType::Count:
                    break;
            }
        }

        int eventSelectResult = ::WSAEventSelect(HandleToType<SOCKET>(socket),
                HandleToType<WSAEVENT>(wsaEvent),mask);

        if (eventSelectResult == SOCKET_ERROR)
        {
            gLastError = std::format("socket event select error: {}", ::WSAGetLastError());
            return false;
        }

        return true;
    }

    void WsaEventReset(WsaEventHandle wsaEvent)
    {
        ::WSAResetEvent(HandleToType<WSAEVENT>(wsaEvent));
    }

    uint32_t WaitForMultipleWsaEvents(const WsaEventHandle* pEventArray, uint32_t numberOfEvents, uint32_t timeOut, bool waitAll, bool alertable)
    {
        if (timeOut == 0)
            timeOut = WSA_INFINITE;

        return ::WSAWaitForMultipleEvents(numberOfEvents, reinterpret_cast<const WSAEVENT*>(pEventArray),
                                          waitAll, timeOut, alertable);
    }

    uint32_t GetWsaEnumEventBaseIndex()
    {
        return WSA_WAIT_EVENT_0;
    }

    std::optional<EnumEventResult> SocketEnumNetworkEvents(SocketHandle socket, WsaEventHandle wsaEvent)
    {
        WSANETWORKEVENTS triggeredEvents;
        int enumResult = ::WSAEnumNetworkEvents(HandleToType<SOCKET>(socket),
                HandleToType<WSAEVENT>(wsaEvent), &triggeredEvents);

        if (enumResult == SOCKET_ERROR)
        {
            gLastError = std::format("socket enum event error: {}", ::WSAGetLastError());
            return std::nullopt;
        }

        EnumEventResult result;

        result[EventType::Read] = GetEnumEventsFdBitResult<FD_READ, FD_READ_BIT>(triggeredEvents);
        result[EventType::Write] = GetEnumEventsFdBitResult<FD_WRITE, FD_WRITE_BIT>(triggeredEvents);
        result[EventType::Accept] = GetEnumEventsFdBitResult<FD_ACCEPT, FD_ACCEPT_BIT>(triggeredEvents);
        result[EventType::Connect] = GetEnumEventsFdBitResult<FD_CONNECT, FD_CONNECT_BIT>(triggeredEvents);
        result[EventType::Close] = GetEnumEventsFdBitResult<FD_CLOSE, FD_CLOSE_BIT>(triggeredEvents);

        return result;
    }
}