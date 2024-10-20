#pragma once

namespace Infra
{
    enum class SocketState
    {
        Success,
        Busy,
        Disconnect,
        Error
    };
}