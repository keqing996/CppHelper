#include "../../Socket.hpp"
#include "../../PlatformDefine.hpp"
#include "../../Windows/WindowsDefine.h"

#if PLATFORM_WINDOWS

#include <WinSock2.h>
#include <ws2ipdef.h>
#include "../../ScopeGuard.hpp"

#pragma comment(lib, "Ws2_32.lib")

namespace Infra::Socket
{


}

#endif