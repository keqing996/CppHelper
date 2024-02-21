#pragma once

#include <vector>
#include <string>
#include <optional>

namespace Infra::File
{
    std::optional<std::vector<char>> LoadBinary(const std::string& filePath);

    std::optional<std::string> LoadText(const std::string& filePath);
}