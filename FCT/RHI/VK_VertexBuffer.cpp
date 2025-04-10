//
// Created by Administrator on 2025/4/9.
//

#include "../FCTAPI.h"

namespace FCT
{
    namespace RHI
    {
        VK_VertexBuffer::VK_VertexBuffer(VK_Context* ctx)
        {
            m_ctx = ctx;
        }

        VK_VertexBuffer::~VK_VertexBuffer()
        {
            if (m_buffer)
            {
                m_ctx->getDevice().destroyBuffer(m_buffer);
                m_buffer = nullptr;
            }
        }

        void VK_VertexBuffer::create()
        {
            vk::BufferCreateInfo bufferInfo{};
            bufferInfo.size = m_size * m_stride;
            bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
            bufferInfo.sharingMode = vk::SharingMode::eExclusive;
            m_buffer = m_ctx->getDevice().createBuffer(bufferInfo);
            vk::MemoryRequirements memRequirements = m_ctx->getDevice().getBufferMemoryRequirements(m_buffer);
            vk::MemoryAllocateInfo allocInfo{};
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = m_ctx->findMemoryType(memRequirements.memoryTypeBits,
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent);
            m_memory = m_ctx->getDevice().allocateMemory(allocInfo);
            m_ctx->getDevice().bindBufferMemory(m_buffer, m_memory, 0);
        }

        void VK_VertexBuffer::mapBuffer(void* data, uint32_t size)
        {
            void* mappedData = m_ctx->getDevice().mapMemory(m_memory, 0, size, vk::MemoryMapFlags());
            memcpy(mappedData, data, size);
            m_ctx->getDevice().unmapMemory(m_memory);
        }

        void VK_VertexBuffer::bind(CommandBuffer* srcCmd, uint32_t slot,uint32_t srcOffset)
        {
            VK_CommandBuffer* cmd = static_cast<VK_CommandBuffer*>(srcCmd);
             vk::DeviceSize offset = static_cast<vk::DeviceSize>(srcOffset);
            cmd->commandBuffer().bindVertexBuffers(0, 1, &m_buffer,&offset);
        }

    }
}