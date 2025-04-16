//
// Created by Administrator on 2025/4/15.
//

#ifndef FCT_RHI_VK_SAMPLER_H
#define FCT_RHI_VK_SAMPLER_H

#include "Sampler.h"

namespace FCT
{
    class VK_Context;
    namespace RHI
    {
        class VK_Sampler : public Sampler {
        public:
            VK_Sampler(VK_Context* ctx);
            ~VK_Sampler();

            void create() override;

            vk::Sampler getSampler() const { return m_sampler; }

        private:
            VK_Context* m_ctx;
            vk::Sampler m_sampler;

            vk::Filter toVkFilter(FilterMode mode) const;
            vk::SamplerMipmapMode toVkMipmapMode(FilterMode mode) const;
            vk::SamplerAddressMode toVkAddressMode(AddressMode mode) const;
            vk::CompareOp toVkCompareOp(CompareOp op) const;
            vk::BorderColor toVkBorderColor(BorderColor color) const;
        };
    }
}
#endif //FCT_RHI_VK_SAMPLER_H