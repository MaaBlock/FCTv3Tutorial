//
// Created by Administrator on 2025/3/31.
//
#include "./CommandBuffer.h"
#include "VK_CommandPool.h"
#ifndef VK_COMMANDBUFFER_H
#define VK_COMMANDBUFFER_H

namespace FCT
{
    namespace RHI
    {
        class VK_CommandBuffer : public CommandBuffer{
        public:
            VK_CommandBuffer(VK_CommandPool* pool);
            void create() override;
        protected:
            VK_CommandPool* m_pool;
            vk::CommandBufferAllocateInfo m_allocateInfo;
            std::vector<vk::CommandBuffer> m_commandBuffers;
        };
    }
}



#endif //VK_COMMANDBUFFER_H
