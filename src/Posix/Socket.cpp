#include "Infra/Windows/WindowsDefine.hpp"

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
    std::optional<Socket::EndPoint<Socket::AddressFamily::IpV4>> Socket::EndPoint<Socket::AddressFamily::IpV4>::TryCreate(const std::string& ip, uint16_t port)
    {
    }
}

#endif