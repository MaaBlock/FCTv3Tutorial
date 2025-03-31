//
// Created by Administrator on 2025/3/31.
//

#include "../FCTAPI.h"
namespace FCT
{
    namespace RHI
    {
        VK_CommandPool::VK_CommandPool(VK_Context* ctx) : m_createInfo{}
        {
            m_ctx = ctx;
        }

        VK_Context* VK_CommandPool::context() const
        {
            return m_ctx;
        }

        void VK_CommandPool::create()
        {
            m_createInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
            m_createInfo.queueFamilyIndex = m_ctx->getGraphicsQueueFamily();
            m_pool = m_ctx->device().createCommandPool(m_createInfo);
        }

        CommandBuffer* VK_CommandPool::createCommandBuffer()
        {
            return new VK_CommandBuffer(this);
        }
    }
}
