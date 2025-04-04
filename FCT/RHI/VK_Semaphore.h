//
// Created by Administrator on 2025/4/2.
//
#include "./Semaphore.h"
#ifndef VK_SEMAPHORE_H
#define VK_SEMAPHORE_H

namespace FCT
{
    namespace RHI
    {
        class VK_Semaphore : public Semaphore{
        public:
            VK_Semaphore(VK_Context* ctx);
            void create() override;
            vk::Semaphore semaphore()
            {
                return m_semaphore;
            }
        private:
            VK_Context* m_ctx;
            vk::Semaphore m_semaphore;
        };
    }
}



#endif //VK_SEMAPHORE_H
