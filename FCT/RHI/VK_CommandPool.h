//
// Created by Administrator on 2025/3/31.
//
#include "./CommandPool.h"
#ifndef VK_COMMANDPOOL_H
#define VK_COMMANDPOOL_H

namespace FCT
{
    namespace RHI
    {
        class VK_CommandPool : public CommandPool {
        public:
            VK_CommandPool(VK_Context* ctx);
            VK_Context* context() const;
            void create() override;
            CommandBuffer* createCommandBuffer() override;
            vk::CommandPool pool() const { return m_pool; }
        private:
            VK_Context* m_ctx;
            vk::CommandPoolCreateInfo m_createInfo;
            vk::CommandPool m_pool;
        };

    }
}


#endif //VK_COMMANDPOOL_H
