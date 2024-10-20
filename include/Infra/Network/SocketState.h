#pragma once

namespace Infra
{
    enum class SocketState
    {
        Success,
        Busy,
        AddrFamilyNotMatch,
        Disconnect,
        Error
    };
}