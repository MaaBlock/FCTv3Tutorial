//
// Created by Administrator on 2025/4/14.
//


#include "../FCTAPI.h"
namespace FCT
{
    namespace RHI
    {
        VK_DescriptorPool::VK_DescriptorPool(VK_Context* ctx)
        {
            m_ctx = ctx;
        }

        void VK_DescriptorPool::create()
        {
            std::vector<vk::DescriptorPoolSize> poolSizes = {
                {vk::DescriptorType::eUniformBuffer, m_ctx->maxFrameInFlight() * 100},

                {vk::DescriptorType::eCombinedImageSampler, m_ctx->maxFrameInFlight() * 100},
                {vk::DescriptorType::eStorageBuffer, m_ctx->maxFrameInFlight() * 50},
                {vk::DescriptorType::eStorageImage, m_ctx->maxFrameInFlight() * 50},
                {vk::DescriptorType::eSampledImage, m_ctx->maxFrameInFlight() * 100},
                {vk::DescriptorType::eSampler, m_ctx->maxFrameInFlight() * 50}
            };

            vk::DescriptorPoolCreateInfo poolInfo{};
            poolInfo.maxSets = 1000;
            poolInfo.setPoolSizes(poolSizes);

            poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

            m_descriptorPool = m_ctx->device().createDescriptorPool(poolInfo);
        }
    }
}