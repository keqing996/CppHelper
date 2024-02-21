#pragma once

#include <string>
#include <memory>
#include <functional>
#include <optional>

#include "PlatformDefine.h"

namespace Infra
{
    class System
    {
    public:
        System() = delete;

    public:
        static auto GetMachineName() -> std::string;
        static auto GetCurrentUserName() -> std::string;

    public:
        static auto GetEnviromentVariable(const std::string& keyName) -> std::string;
        static auto SetEnviromentVariable(const std::string& keyName, const std::string& value) -> void;

    public:
        static auto GetHomeDirectory() -> std::string;
        static auto GetCurrentDirectory() -> std::string;
        static auto SetCurrentDirectory(const std::string& path) -> bool;
        static auto GetExecutableDirectory() -> std::string;
        static auto GetTempDirectory() -> std::string;
    };
}