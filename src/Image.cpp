
#include <memory>
#include "Infra/ScopeGuard.hpp"
#include "Infra/Image.hpp"
#include "ThirdParty/stb_image.h"

namespace Infra
{
   Image::Image(uint width, uint height, std::uint8_t r, std::uint8_t g, std::uint8_t b)
        : Image(width, height, r, g, b, 255)
    {
    }

    Image::Image(uint width, uint height, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a)
    {
        if (width == 0 || height == 0)
        {
            _width = 0;
            _height = 0;
            _data.clear();
            return;
        }

       _width = width;
       _height = height;

        uint32_t color;
        uint8_t* pColor = reinterpret_cast<uint8_t*>(&color);
        pColor[0] = r;
        pColor[1] = g;
        pColor[2] = b;
        pColor[3] = a;

        size_t pixelSize = static_cast<std::size_t>(_width) * static_cast<std::size_t>(_height);

        std::vector<std::uint8_t> newData(pixelSize * 4);
        uint32_t* pData = reinterpret_cast<uint32_t*>(newData.data());
        for (auto i = 0; i < pixelSize; i++)
            *(pData + i) = color;

        // create buffer and swap, incase exception
        _data.swap(newData);
    }

    Image::Image(uint width, uint height, std::uint8_t* pData)
    {
        CopyFromData(width, height, pData);
    }

    Image::Image(const std::string& filePath)
    {
        int width = 0;
        int height = 0;
        int channels = 0;
        stbi_uc* pStbImage = ::stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (pStbImage == nullptr)
            return;

        // in case exception
        ScopeGuard fileStreamGuard = [&] { ::stbi_image_free(pStbImage); };

        CopyFromData(width, height, pStbImage);
    }

    Image::Image(const void* pMemoryData, size_t dataSize)
    {
       if (pMemoryData == nullptr || dataSize == 0)
           return;

       int width = 0;
       int height = 0;
       int channels = 0;
       stbi_uc* pStbImage = ::stbi_load_from_memory(
           static_cast<const unsigned char*>(pMemoryData),
           static_cast<int>(dataSize), &width, &height,
           &channels, STBI_rgb_alpha);

       if (pStbImage == nullptr)
           return;

       // in case exception
       ScopeGuard fileStreamGuard = [&] { ::stbi_image_free(pStbImage); };

       CopyFromData(width, height, pStbImage);
    }

    void Image::CopyFromData(uint width, uint height, std::uint8_t* pData)
    {
        if (pData == nullptr)
        {
            _width = 0;
            _height = 0;
            _data.clear();
            return;
        }

        _width = width;
        _height = height;

        size_t pixelSize = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
        std::vector<std::uint8_t> newData(pData, pData + pixelSize * 4);

        // create buffer and swap, incase exception
        _data.swap(newData);
    }
}
