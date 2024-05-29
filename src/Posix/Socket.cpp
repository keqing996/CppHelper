#include "Infra/PlatformDefine.hpp"

#if PLATFORM_SUPPORT_POSIX

#include "Infra/Socket.hpp"
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

    }

    auto Socket::DestroySocket(const SocketHandle& socketHandle) -> void
    {

    }
}

#endif