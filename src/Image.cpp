

#include "Infra/Image.hpp"
#include "ThirdParty/stb_image.h"

namespace Infra
{
    Image::Image(uint width, uint height)
        : Image(width, height, 0, 0, 0, 255)
    {
    }

    Image::Image(uint width, uint height, std::uint8_t r, std::uint8_t g, std::uint8_t b)
        : Image(width, height, r, g, b, 255)
    {
    }

    Image::Image(uint width, uint height, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
        : _width(width)
        , _height(height)
    {
        size_t pixelSize = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
        _data.resize(pixelSize * 4);

        uint32_t color;
        uint8_t* pColor = reinterpret_cast<uint8_t*>(&color);
        pColor[0] = r;
        pColor[1] = g;
        pColor[2] = b;
        pColor[3] = a;

        uint32_t* pData = reinterpret_cast<uint32_t*>(_data.data());
        for (auto i = 0; i < pixelSize; i++)
            *(pData + i) = color;
    }

    Image::Image(uint width, uint height, std::uint8_t* pData)
        : _width(width)
        , _height(height)
    {
        if (pData == nullptr)
        {
            _width = 0;
            _height = 0;
            _data.clear();
            return;
        }

        size_t pixelSize = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
        std::vector<std::uint8_t> newData(pData, pData + pixelSize * 4);
        _data.swap(newData);
    }
}
