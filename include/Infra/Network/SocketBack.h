namespace Infra
{
    struct SocketHandle
    {
        void* handle = nullptr;
        AddressFamily family;
        Protocol protocol;
    };

    class Socket
    {
    public:
        Socket() = delete;

    public:
        // Init and clear
        static auto IsInitialized() -> bool;
        static auto InitEnvironment() -> bool;
        static auto DestroyEnvironment() -> void;

        // Create and destroy
        static auto CreateEndpoint(const std::string& ipStr, uint16_t port) -> std::optional<EndPoint<AddressFamily::IpV4>>;
        static auto CreateSocket(AddressFamily family, Protocol protocol) -> std::optional<SocketHandle>;
        static auto DestroySocket(const SocketHandle& socketHandle) -> void;

        // Client
        template<AddressFamily addrFamily>
        static auto Connect(const SocketHandle& socketHandle, const EndPoint<addrFamily>& endpoint, int timeOutInMs = -1) -> bool;

        // Server
        template<AddressFamily addrFamily>
        static auto Bind(const SocketHandle& socketHandle, const EndPoint<addrFamily>& endpoint) -> bool;
        static auto Listen(const SocketHandle& socketHandle) -> bool;
        static auto Accept(const SocketHandle& socketHandle, int timeOutInMs = -1) -> std::optional<SocketHandle>;

        // Send & Recv
        static auto Send(const SocketHandle& socketHandle, const char* pDataBuffer, int bufferSize) -> bool;
        static auto Receive(const SocketHandle& socketHandle, char* pDataBuffer, int bufferSize, int timeOutInMs = -1) -> std::optional<int>;

    };

}