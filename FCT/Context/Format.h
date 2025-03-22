//
// Created by Administrator on 2025/3/5.
//
#include "../ThirdParty.h"

#ifndef FCT_FORMAT_H
#define FCT_FORMAT_H
namespace FCT {
    enum class Format {
        UNDEFINED,
        R8_UNORM,
        R8G8_UNORM,
        R8G8B8_UNORM,
        R8G8B8A8_UNORM,
        B8G8R8A8_UNORM,
        R8G8B8A8_SRGB,
        B8G8R8A8_SRGB,

        // 16位格式
        R16_SFLOAT,
        R16G16_SFLOAT,
        R16G16B16A16_SFLOAT,

        // 32位格式
        R32_SFLOAT,
        R32G32_SFLOAT,
        R32G32B32_SFLOAT,
        R32G32B32A32_SFLOAT,

        // 深度/模板格式
        D16_UNORM,
        D24_UNORM_S8_UINT,
        D32_SFLOAT,
        D32_SFLOAT_S8_UINT
    };

    inline vk::Format ToVkFormat(Format format) {
        switch (format) {
            case Format::UNDEFINED:
                return vk::Format::eUndefined;
            case Format::R8_UNORM:
                return vk::Format::eR8Unorm;
            case Format::R8G8_UNORM:
                return vk::Format::eR8G8Unorm;
            case Format::R8G8B8_UNORM:
                return vk::Format::eR8G8B8Unorm;
            case Format::R8G8B8A8_UNORM:
                return vk::Format::eR8G8B8A8Unorm;
            case Format::B8G8R8A8_UNORM:
                return vk::Format::eB8G8R8A8Unorm;
            case Format::R8G8B8A8_SRGB:
                return vk::Format::eR8G8B8A8Srgb;
            case Format::B8G8R8A8_SRGB:
                return vk::Format::eB8G8R8A8Srgb;
            case Format::R16_SFLOAT:
                return vk::Format::eR16Sfloat;
            case Format::R16G16_SFLOAT:
                return vk::Format::eR16G16Sfloat;
            case Format::R16G16B16A16_SFLOAT:
                return vk::Format::eR16G16B16A16Sfloat;
            case Format::R32_SFLOAT:
                return vk::Format::eR32Sfloat;
            case Format::R32G32_SFLOAT:
                return vk::Format::eR32G32Sfloat;
            case Format::R32G32B32_SFLOAT:
                return vk::Format::eR32G32B32Sfloat;
            case Format::R32G32B32A32_SFLOAT:
                return vk::Format::eR32G32B32A32Sfloat;
            case Format::D16_UNORM:
                return vk::Format::eD16Unorm;
            case Format::D24_UNORM_S8_UINT:
                return vk::Format::eD24UnormS8Uint;
            case Format::D32_SFLOAT:
                return vk::Format::eD32Sfloat;
            case Format::D32_SFLOAT_S8_UINT:
                return vk::Format::eD32SfloatS8Uint;
            default:
                return vk::Format::eUndefined;
        }
    }
}


#endif //FCT_FORMAT_H
