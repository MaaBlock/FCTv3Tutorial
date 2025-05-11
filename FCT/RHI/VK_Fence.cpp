//
// Created by Administrator on 2025/4/2.
//

#include "../FCTAPI.h"
namespace FCT
{
    namespace RHI
    {
        VK_Fence::VK_Fence(VK_Context* ctx)
        {
            m_ctx = ctx;
        }

        void VK_Fence::reset()
        {
            m_ctx->device().resetFences(1, &m_fence);
        }

        void VK_Fence::create()
        {
            addRef();
            vk::FenceCreateInfo createInfo{};
            createInfo.flags |= m_signaled ? vk::FenceCreateFlagBits::eSignaled : vk::FenceCreateFlags(0);
            m_fence = m_ctx->device().createFence(createInfo);
        }

        void VK_Fence::waitFor()
        {
            m_ctx->device().waitForFences(1, &m_fence, VK_TRUE, UINT64_MAX);
        }

        void VK_Fence::destroy()
        {
            if (m_fence)
            {
                m_ctx->device().destroyFence(m_fence, nullptr);
                m_fence = nullptr;
            }
        }
    }
}
