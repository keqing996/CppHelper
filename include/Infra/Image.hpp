#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace Infra
{
    class Image
    {
        using uint = unsigned int;

    public:
        Image(uint width, uint height, std::uint8_t r, std::uint8_t g, std::uint8_t b);
        Image(uint width, uint height, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a);
        Image(uint width, uint height, std::uint8_t* pData);

        // Load form file
        explicit Image(const std::string& filePath);

        // Load from memory
        Image(const void* pMemoryData, size_t dataSize);

    private:
        void CopyFromData(uint width, uint height, std::uint8_t* pData);

    private:
        std::vector<std::uint8_t> _data;
        uint _width = 0;
        uint _height = 0;
    };
}