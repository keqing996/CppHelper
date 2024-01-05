#pragma once

#include <string>
#include <thread>
#include <optional>
#include <array>

namespace Helper::Win::Socket
{
    using Byte = char;
    using SocketHandle = void*;
    using WsaEventHandle = void*;

    enum class EventType : int
    {
        Read,
        Write,
        Accept,
        Connect,
        Close,
        Count
    };

    struct SocketAddr
    {
        std::string ip;
        unsigned int port;
    };

    class EnumEventResult
    {
    public:
        EnumEventResult();
        bool operator[] (EventType t) const;
        bool& operator[] (EventType t);

    private:
        std::array<bool, static_cast<int>(EventType::Count)> _events;
    };

    const std::string& LastErrorMessage();

    bool StartUp();

    void CleanUp();

    std::optional<SocketHandle> CreateIpv4TcpSocket();

    void CloseSocket(SocketHandle socket);

    bool Send(SocketHandle socket, Byte* pDataBuffer, int bufferSize);

    std::optional<int> Receive(SocketHandle socket, Byte* pDataBuffer, int bufferSize);

    bool Connect(SocketHandle socket, const std::string& ipStr, int port);

    bool Bind(SocketHandle socket, const std::string& ipStr, int port);

    bool Listen(SocketHandle socket);

    std::optional<SocketAddr> Accept(SocketHandle socket);

    std::optional<WsaEventHandle> CreateWsaEvent();

    void CloseWsaEvent(WsaEventHandle wsaEvent);

    bool WsaEventSelect(SocketHandle socket, WsaEventHandle wsaEvent, const EventType* events, uint32_t eventNum);

    void WsaEventReset(WsaEventHandle wsaEvent);

    uint32_t WaitForMultipleWsaEvents(const WsaEventHandle* pEventArray, uint32_t numberOfEvents, uint32_t timeOut = 0, bool waitAll = false, bool alertable = false);

    uint32_t GetWsaEnumEventBaseIndex();

    std::optional<EnumEventResult> SocketEnumNetworkEvents(SocketHandle socket, WsaEventHandle wsaEvent);

}