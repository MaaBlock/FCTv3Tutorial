//
// Created by Administrator on 2025/5/9.
//
#ifndef VK_SEMAPHOREPOOL_H
#define VK_SEMAPHOREPOOL_H
#include "../Memory/ObjectPool.h"
#include "./SemaphorePool.h"
namespace FCT {
    class VK_SemaphorePool : public SemaphorePool
    {
    protected:
        ObjectPool<RHI::VK_Semaphore,VK_Context*> m_semaphorePool;
    public:
        VK_SemaphorePool(VK_Context* ctx) : m_semaphorePool(ctx)
        {
            m_semaphorePool.setDestroyCallback([](RHI::VK_Semaphore* semaphore)
            {
                semaphore->destroy();
            });
        }
        void setDestroyCallback(const std::function<void(RHI::Semaphore*)>& callback) override
        {
            m_semaphorePool.setDestroyCallback([callback](RHI::VK_Semaphore* semaphore)
            {
                semaphore->destroy();
                callback(semaphore);
            });
        }
        RHI::Semaphore* alloc() override
        {
            return m_semaphorePool.alloc();
        }
    };
}
#endif //VK_SEMAPHOREPOOL_H
