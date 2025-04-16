#include "../FCTAPI.h"

namespace FCT
{
    namespace RHI
    {
        VK_Sampler::VK_Sampler(VK_Context* ctx)
            : m_ctx(ctx), m_sampler(nullptr)
        {
        }

        VK_Sampler::~VK_Sampler()
        {
            if (m_sampler) {
                m_ctx->getDevice().destroySampler(m_sampler);
                m_sampler = nullptr;
            }
        }

        void VK_Sampler::create()
        {
            if (m_sampler) {
                m_ctx->getDevice().destroySampler(m_sampler);
                m_sampler = nullptr;
            }

            vk::SamplerCreateInfo samplerInfo;
            samplerInfo.setMagFilter(toVkFilter(m_magFilter));
            samplerInfo.setMinFilter(toVkFilter(m_minFilter));
            samplerInfo.setMipmapMode(toVkMipmapMode(m_mipmapFilter));

            samplerInfo.setAddressModeU(toVkAddressMode(m_addressModeU));
            samplerInfo.setAddressModeV(toVkAddressMode(m_addressModeV));
            samplerInfo.setAddressModeW(toVkAddressMode(m_addressModeW));

            samplerInfo.setMipLodBias(m_mipLodBias);
            samplerInfo.setAnisotropyEnable(m_anisotropyEnable);
            samplerInfo.setMaxAnisotropy(m_maxAnisotropy);

            samplerInfo.setCompareEnable(m_compareEnable);
            samplerInfo.setCompareOp(toVkCompareOp(m_compareOp));

            samplerInfo.setMinLod(m_minLod);
            samplerInfo.setMaxLod(m_maxLod);

            samplerInfo.setBorderColor(toVkBorderColor(m_borderColor));
            samplerInfo.setUnnormalizedCoordinates(m_unnormalizedCoordinates);

            try {
                m_sampler = m_ctx->getDevice().createSampler(samplerInfo);
            }
            catch (const std::exception& e) {
                ferr << "Failed to create sampler: " << e.what() << std::endl;
                m_sampler = nullptr;
            }
        }

        vk::Filter VK_Sampler::toVkFilter(FilterMode mode) const
        {
            switch (mode) {
                case FilterMode::Nearest:
                    return vk::Filter::eNearest;
                case FilterMode::Linear:
                    return vk::Filter::eLinear;
                default:
                    return vk::Filter::eLinear;
            }
        }

        vk::SamplerMipmapMode VK_Sampler::toVkMipmapMode(FilterMode mode) const
        {
            switch (mode) {
                case FilterMode::Nearest:
                    return vk::SamplerMipmapMode::eNearest;
                case FilterMode::Linear:
                    return vk::SamplerMipmapMode::eLinear;
                default:
                    return vk::SamplerMipmapMode::eLinear;
            }
        }

        vk::SamplerAddressMode VK_Sampler::toVkAddressMode(AddressMode mode) const
        {
            switch (mode) {
                case AddressMode::Repeat:
                    return vk::SamplerAddressMode::eRepeat;
                case AddressMode::MirroredRepeat:
                    return vk::SamplerAddressMode::eMirroredRepeat;
                case AddressMode::ClampToEdge:
                    return vk::SamplerAddressMode::eClampToEdge;
                case AddressMode::ClampToBorder:
                    return vk::SamplerAddressMode::eClampToBorder;
                case AddressMode::MirrorClampToEdge:
                    return vk::SamplerAddressMode::eMirrorClampToEdge;
                default:
                    return vk::SamplerAddressMode::eRepeat;
            }
        }

        vk::CompareOp VK_Sampler::toVkCompareOp(CompareOp op) const
        {
            switch (op) {
                case CompareOp::Never:
                    return vk::CompareOp::eNever;
                case CompareOp::Less:
                    return vk::CompareOp::eLess;
                case CompareOp::Equal:
                    return vk::CompareOp::eEqual;
                case CompareOp::LessOrEqual:
                    return vk::CompareOp::eLessOrEqual;
                case CompareOp::Greater:
                    return vk::CompareOp::eGreater;
                case CompareOp::NotEqual:
                    return vk::CompareOp::eNotEqual;
                case CompareOp::GreaterOrEqual:
                    return vk::CompareOp::eGreaterOrEqual;
                case CompareOp::Always:
                    return vk::CompareOp::eAlways;
                default:
                    return vk::CompareOp::eNever;
            }
        }

        vk::BorderColor VK_Sampler::toVkBorderColor(BorderColor color) const
        {
            switch (color) {
                case BorderColor::TransparentBlack:
                    return vk::BorderColor::eFloatTransparentBlack;
                case BorderColor::OpaqueBlack:
                    return vk::BorderColor::eFloatOpaqueBlack;
                case BorderColor::OpaqueWhite:
                    return vk::BorderColor::eFloatOpaqueWhite;
                default:
                    return vk::BorderColor::eFloatOpaqueBlack;
            }
        }
    }
}