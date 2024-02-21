#include "../../File.h"
#include "../../ScopeGuard.h"

#include <fstream>
#include <filesystem>
#include <sstream>

namespace Infra::File
{
    std::optional<std::vector<char>> LoadBinary(const std::string& filePath)
    {
        if (!std::filesystem::exists(filePath))
            return std::nullopt;

        std::ifstream fileStream(filePath, std::ios::binary);
        if (!fileStream.is_open())
            return std::nullopt;

        ScopeGuard fileStreamGuard = [&] { fileStream.close(); };
        {
            fileStream.seekg(0, std::ios::end);
            unsigned int size = fileStream.tellg();
            std::vector<char> content(size);

            fileStream.seekg(0, std::ios::beg);
            fileStream.read(content.data(), size);

            return content;
        }
    }

    std::optional<std::string> LoadText(const std::string& filePath)
    {
        if (!std::filesystem::exists(filePath))
            return std::nullopt;

        std::ifstream fileStream(filePath, std::ios::binary);
        if (!fileStream.is_open())
            return std::nullopt;

        ScopeGuard fileStreamGuard = [&] { fileStream.close(); };
        {
            std::ostringstream stringStream;
            stringStream << fileStream.rdbuf();

            return stringStream.str();
        }
    }
}