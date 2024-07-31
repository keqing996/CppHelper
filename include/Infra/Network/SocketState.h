#pragma once

namespace Infra
{
    enum class SocketState
    {
        Success,
        InProgress,
        AddrFamilyNotMatch,
        Disconnect,
        Error
    };
}