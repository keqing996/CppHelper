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
        struct Pixel
        {
            std::uint8_t r;
            std::uint8_t g;
            std::uint8_t b;
            std::uint8_t a;

            Pixel();
            Pixel(uint32_t packColor);
            Pixel(std::uint8_t red, std::uint8_t green, std::uint8_t blue);
            Pixel(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha);

            uint32_t Pack() const;
        };

    public:
        Image(uint width, uint height, Pixel pixel);
        Image(uint width, uint height, std::uint8_t* pData);

        // Load form file
        explicit Image(const std::string& filePath);

        // Load from memory
        Image(const void* pMemoryData, size_t dataSize);

    public:
        std::pair<uint, uint> GetSize() const;

        Pixel GetPixel(uint x, uint y) const;

        void SetPixel(uint x, uint y, Pixel pixel);

    private:
        void CopyFromData(uint width, uint height, std::uint8_t* pData);

    private:
        std::vector<std::uint8_t> _data;
        uint _width = 0;
        uint _height = 0;
    };
}