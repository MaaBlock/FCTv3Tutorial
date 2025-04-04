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
            vk::SemaphoreCreateInfo semaphoreInfo;
            m_semaphore = m_ctx->device().createSemaphore(semaphoreInfo);
        }
    }
}
