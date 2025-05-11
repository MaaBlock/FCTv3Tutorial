//
// Created by Administrator on 2025/5/10.
//

#ifndef VK_FENCEPOOL_H
#define VK_FENCEPOOL_H
#include "../Memory/ObjectPool.h"
#include "./FencePool.h"
namespace FCT
{
    class VK_FencePool : public FencePool
    {
    protected:
        ObjectPool<RHI::VK_Fence,VK_Context*> m_fencePool;
    public:
        VK_FencePool(VK_Context* ctx) : m_fencePool(ctx)
        {
            m_fencePool.setDestroyCallback([](RHI::VK_Fence* fence)
            {
                fence->destroy();
            });
        }
        void setDestroyCallback(const std::function<void(RHI::Fence*)>& callback) override
        {
            m_fencePool.setDestroyCallback([callback](RHI::VK_Fence* fence)
            {
                fence->destroy();
                callback(fence);
            });
        }
        RHI::Fence* alloc()
        {
            return m_fencePool.alloc();
        }
    };

}
#endif //VK_FENCEPOOL_H
