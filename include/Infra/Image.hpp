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
        Image(uint width, uint height);
        Image(uint width, uint height, std::uint8_t r, std::uint8_t g, std::uint8_t b);
        Image(uint width, uint height, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a);
        Image(uint width, uint height, std::uint8_t* pData);

    public:


    private:
        std::vector<std::uint8_t> _data;
        uint _width;
        uint _height;
    };
}