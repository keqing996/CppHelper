#include "Infra/PlatformDefine.h"

#if PLATFORM_SUPPORT_POSIX

#include "Infra/Socket.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/stat.h>

namespace Infra
{
    namespace Detail
    {
        static int GetAddressFamily(Infra::AddressFamily family)
        {
            switch (family)
            {
                case Infra::AddressFamily::IpV4:
                    return AF_INET;
                case Infra::AddressFamily::IpV6:
                    return AF_INET6;
            }

            return AF_INET;
        }

        static int GetProtocol(Infra::Protocol protocol)
        {
            switch (protocol)
            {
                case Infra::Protocol::Tcp:
                    return SOCK_STREAM;
                case Infra::Protocol::Udp:
                    return SOCK_DGRAM;
            }

            return SOCK_STREAM;
        }

        static bool ConnectInternal(const Infra::SocketHandle& socketHandle, const sockaddr* pAddr, int size, int timeOutInMs)
        {

        }
    }

    auto Socket::IsInitialized() -> bool
    {
        return true;
    }

    auto Socket::InitEnvironment() -> bool
    {
        // do nothing
        return true;
    }

    auto Socket::DestroyEnvironment() -> void
    {
        // do nothing
    }

    auto Socket::CreateEndpoint(const std::string& ipStr, uint16_t port) -> std::optional<EndPoint<AddressFamily::IpV4>>
    {
        char buffer[INET_ADDRSTRLEN];
        int ret = ::inet_pton(AF_INET, ipStr.c_str(), buffer);
        if (ret <= 0)
            return std::nullopt;

        uint32_t ip = std::stoul(buffer);
        return EndPoint<AddressFamily::IpV4>(ip, port);
    }

    auto Socket::CreateSocket(AddressFamily family, Protocol protocol) -> std::optional<SocketHandle>
    {
        int af = Detail::GetAddressFamily(family);
        int po = Detail::GetProtocol(protocol);
        int rt = ::socket(af, po, 0);
        if (rt < 0)
            return std::nullopt;

        return SocketHandle{ reinterpret_cast<void*>(rt), family, protocol };
    }

    auto Socket::DestroySocket(const SocketHandle& socketHandle) -> void
    {
        ::close(reinterpret_cast<int>(socketHandle.handle));
    }
}

#endif