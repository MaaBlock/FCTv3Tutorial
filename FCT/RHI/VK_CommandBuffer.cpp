//
// Created by Administrator on 2025/3/31.
//

#include "../FCTAPI.h"
namespace FCT
{
    namespace RHI
    {
        VK_CommandBuffer::VK_CommandBuffer(VK_CommandPool* pool) : m_allocateInfo{}
        {
            m_pool = pool;
        }

        void VK_CommandBuffer::create()
        {
            switch (m_level)
            {
            case CommandBufferLevel::Primary:
                m_allocateInfo.level = vk::CommandBufferLevel::ePrimary;
                break;
            case CommandBufferLevel::Secondary:
                m_allocateInfo.level = vk::CommandBufferLevel::eSecondary;
                break;
            }
            m_allocateInfo.commandPool = m_pool->pool();
            m_allocateInfo.commandBufferCount = 1;
            m_commandBuffers = m_pool->context()->device().allocateCommandBuffers(m_allocateInfo);
        }
    }
}
