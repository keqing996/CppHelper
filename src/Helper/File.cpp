#include "File.h"

#include <fstream>
#include <filesystem>

namespace Helper::File
{
    std::vector<char> LoadBinary(const std::string& filePath)
    {
        if (!std::filesystem::exists(filePath))
            return {};

        std::ifstream fileStream(filePath, std::ios::binary);
        if (!fileStream.is_open())
            return {};

        fileStream.seekg(0, std::ios::end);
        unsigned int size = fileStream.tellg();
        std::vector<char> content(size);

        fileStream.seekg(0, std::ios::beg);
        fileStream.read(content.data(), size);
        fileStream.close();

        return content;
    }
}