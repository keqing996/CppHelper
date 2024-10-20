
#include "Infra/Network/Socket.h"
#include "Windows/WindowsSocket.h"
#include "Posix/PosixSocket.h"

namespace Infra
{
    Socket::Socket(IpAddress::Family af, void* handle)
        : _addressFamily(af)
        , _handle(handle)
        , _isBlocking(false)
    {
    }

    void* Socket::GetNativeHandle() const
    {
        return _handle;
    }

    bool Socket::IsBlocking() const
    {
        return _isBlocking;
    }

    bool Socket::SetBlocking(bool block, bool force)
    {
        if (!force && block == IsBlocking())
            return true;

        return Device::SetSocketBlocking(_handle, block);
    }

    SocketState Socket::SelectRead(const Socket* pSocket, int timeoutInMs)
    {
        if (timeoutInMs <= 0)
            return SocketState::Error;

        if (pSocket == nullptr)
            return SocketState::Error;

        void* handle = pSocket->GetNativeHandle();

        fd_set selector;
        FD_ZERO(&selector);
        FD_SET(Device::ToNativeHandle(handle), &selector);

        timeval time{};
        time.tv_sec  = static_cast<long>(timeoutInMs / 1000);
        time.tv_usec = timeoutInMs % 1000 * 1000;

        if (::select(static_cast<int>(Device::ToNativeHandle(handle) + 1), &selector, nullptr, nullptr, &time) > 0)
            return SocketState::Success;

        return Device::GetErrorState();
    }

    SocketState Socket::SelectWrite(const Socket* pSocket, int timeoutInMs)
    {
        if (timeoutInMs <= 0)
            return SocketState::Error;

        if (pSocket == nullptr)
            return SocketState::Error;

        void* handle = pSocket->GetNativeHandle();

        fd_set selector;
        FD_ZERO(&selector);
        FD_SET(Device::ToNativeHandle(handle), &selector);

        timeval time{};
        time.tv_sec  = static_cast<long>(timeoutInMs / 1000);
        time.tv_usec = timeoutInMs % 1000 * 1000;

        if (::select(static_cast<int>(Device::ToNativeHandle(handle) + 1), nullptr, &selector, nullptr, &time) > 0)
            return SocketState::Success;

        return Device::GetErrorState();
    }

    SocketState Socket::SelectRead(int timeoutInMs)
    {
        return SelectRead(this, timeoutInMs);
    }

    SocketState Socket::SelectWrite(int timeoutInMs)
    {
        return SelectWrite(this, timeoutInMs);
    }
}
