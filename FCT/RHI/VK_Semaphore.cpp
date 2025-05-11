//
// Created by Administrator on 2025/4/2.
//

#include "../FCTAPI.h"

namespace FCT {
    namespace RHI
    {
        VK_Semaphore::VK_Semaphore(VK_Context* ctx) : m_ctx(ctx)
        {
        }

        void VK_Semaphore::create()
        {
            addRef();
            if (m_semaphore)
            {
                m_ctx->device().waitIdle();
                m_ctx->device().destroySemaphore(m_semaphore);
                m_semaphore = nullptr;
            }
            vk::SemaphoreCreateInfo semaphoreInfo;
            m_semaphore = m_ctx->device().createSemaphore(semaphoreInfo);
        }

        void VK_Semaphore::destroy()
        {
            if (m_semaphore)
            {
                m_ctx->device().destroySemaphore(m_semaphore);
                m_semaphore = nullptr;
            }
        }
    }
}
