#pragma once

#include <vector>
#include <string>

namespace Helper::File
{
    std::vector<char> LoadBinary(const std::string& filePath);

    std::string LoadText(const std::string& filePath);
}