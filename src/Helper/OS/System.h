#pragma once

#include <string>
#include "../PlatformDefine.h"

namespace Helper::OS::System
{
    std::string GetMachineName();

    std::string GetCurrentUserName();

    std::string GetEnvVariable(const std::string& keyName);

    void SetEnvVariable(const std::string& keyName, const std::string& value);

    std::string GetHomeDirectory();

}