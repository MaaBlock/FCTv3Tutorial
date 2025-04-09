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
    constexpr const char* FormatToShaderType(Format format) noexcept
    {
        switch (format) {
        case Format::R8_UNORM:
        case Format::R16_SFLOAT:
        case Format::R32_SFLOAT:
            return "float";

        case Format::R8G8_UNORM:
        case Format::R16G16_SFLOAT:
        case Format::R32G32_SFLOAT:
            return "float2";

        case Format::R8G8B8_UNORM:
        case Format::R32G32B32_SFLOAT:
            return "float3";

        case Format::R8G8B8A8_UNORM:
        case Format::B8G8R8A8_UNORM:
        case Format::R8G8B8A8_SRGB:
        case Format::B8G8R8A8_SRGB:
        case Format::R16G16B16A16_SFLOAT:
        case Format::R32G32B32A32_SFLOAT:
            return "float4";

        case Format::D16_UNORM:
        case Format::D32_SFLOAT:
            return "float";

        case Format::D24_UNORM_S8_UINT:
        case Format::D32_SFLOAT_S8_UINT:
            return "float";

        case Format::UNDEFINED:
        default:
            return "float4";
        }
    }
    constexpr size_t FormatSize(Format format) noexcept {
        switch (format) {

        case Format::R8_UNORM:
            return 1;
        case Format::R8G8_UNORM:
            return 2;
        case Format::R8G8B8_UNORM:
            return 3;
        case Format::R8G8B8A8_UNORM:
        case Format::B8G8R8A8_UNORM:
        case Format::R8G8B8A8_SRGB:
        case Format::B8G8R8A8_SRGB:
            return 4;

        case Format::R16_SFLOAT:
            return 2;
        case Format::R16G16_SFLOAT:
            return 4;
        case Format::R16G16B16A16_SFLOAT:
            return 8;

        case Format::R32_SFLOAT:
            return 4;
        case Format::R32G32_SFLOAT:
            return 8;
        case Format::R32G32B32_SFLOAT:
            return 12;
        case Format::R32G32B32A32_SFLOAT:
            return 16;

        case Format::D16_UNORM:
            return 2;
        case Format::D24_UNORM_S8_UINT:
            return 4;
        case Format::D32_SFLOAT:
            return 4;
        case Format::D32_SFLOAT_S8_UINT:
            return 5;

        case Format::UNDEFINED:
        default:
            return 0;
        }
    }
    enum class Samples
    {
        sample_1,
        sample_2,
        sample_4,
        sample_8,
        sample_16,
        sample_32,
    };
#ifdef FCT_USE_VULKAN
    inline vk::SampleCountFlagBits ToVkSampleCount(Samples samples) {
        switch (samples) {
        case Samples::sample_1:
            return vk::SampleCountFlagBits::e1;
        case Samples::sample_2:
            return vk::SampleCountFlagBits::e2;
        case Samples::sample_4:
            return vk::SampleCountFlagBits::e4;
        case Samples::sample_8:
            return vk::SampleCountFlagBits::e8;
        case Samples::sample_16:
            return vk::SampleCountFlagBits::e16;
        case Samples::sample_32:
            return vk::SampleCountFlagBits::e32;
        default:
            return vk::SampleCountFlagBits::e1;
        }
    }

    inline Samples FromVkSampleCount(vk::SampleCountFlagBits sampleCount) {
        switch (sampleCount) {
        case vk::SampleCountFlagBits::e1:
            return Samples::sample_1;
        case vk::SampleCountFlagBits::e2:
            return Samples::sample_2;
        case vk::SampleCountFlagBits::e4:
            return Samples::sample_4;
        case vk::SampleCountFlagBits::e8:
            return Samples::sample_8;
        case vk::SampleCountFlagBits::e16:
            return Samples::sample_16;
        case vk::SampleCountFlagBits::e32:
            return Samples::sample_32;
        default:
            return Samples::sample_1;
        }
    }
#endif
#ifdef FCT_USE_VULKAN
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
    inline Format FromVkFormat(vk::Format format) {
        switch (format) {
        case vk::Format::eUndefined:
            return Format::UNDEFINED;
        case vk::Format::eR8Unorm:
            return Format::R8_UNORM;
        case vk::Format::eR8G8Unorm:
            return Format::R8G8_UNORM;
        case vk::Format::eR8G8B8Unorm:
            return Format::R8G8B8_UNORM;
        case vk::Format::eR8G8B8A8Unorm:
            return Format::R8G8B8A8_UNORM;
        case vk::Format::eB8G8R8A8Unorm:
            return Format::B8G8R8A8_UNORM;
        case vk::Format::eR8G8B8A8Srgb:
            return Format::R8G8B8A8_SRGB;
        case vk::Format::eB8G8R8A8Srgb:
            return Format::B8G8R8A8_SRGB;
        case vk::Format::eR16Sfloat:
            return Format::R16_SFLOAT;
        case vk::Format::eR16G16Sfloat:
            return Format::R16G16_SFLOAT;
        case vk::Format::eR16G16B16A16Sfloat:
            return Format::R16G16B16A16_SFLOAT;
        case vk::Format::eR32Sfloat:
            return Format::R32_SFLOAT;
        case vk::Format::eR32G32Sfloat:
            return Format::R32G32_SFLOAT;
        case vk::Format::eR32G32B32Sfloat:
            return Format::R32G32B32_SFLOAT;
        case vk::Format::eR32G32B32A32Sfloat:
            return Format::R32G32B32A32_SFLOAT;
        case vk::Format::eD16Unorm:
            return Format::D16_UNORM;
        case vk::Format::eD24UnormS8Uint:
            return Format::D24_UNORM_S8_UINT;
        case vk::Format::eD32Sfloat:
            return Format::D32_SFLOAT;
        case vk::Format::eD32SfloatS8Uint:
            return Format::D32_SFLOAT_S8_UINT;
        default:
            return Format::UNDEFINED;
        }
    }
#endif
}


#endif //FCT_FORMAT_H
