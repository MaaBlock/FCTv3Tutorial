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
            vk::DescriptorPoolSize size;
            size.descriptorCount = m_ctx->maxFrameInFlight();
            vk::DescriptorPoolCreateInfo poolInfo{};
            poolInfo.maxSets = 1000;
            poolInfo.setPoolSizes(size);
            m_descriptorPool = m_ctx->device().createDescriptorPool(poolInfo);
        }
    }
}