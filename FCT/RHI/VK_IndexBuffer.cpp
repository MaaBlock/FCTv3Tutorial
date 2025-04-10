//
// Created by Administrator on 2025/4/10.
//
#include "../FCTAPI.h"
namespace FCT
{
    namespace RHI
    {
        VK_IndexBuffer::VK_IndexBuffer(VK_Context* ctx)
        {
            m_ctx = ctx;
        }

        VK_IndexBuffer::~VK_IndexBuffer()
        {
            destroy();
        }

        void VK_IndexBuffer::create()
        {
            if (m_isCreated) {
                destroy();
            }

            uint32_t bufferSize = static_cast<uint32_t>(m_size * m_stride);

            vk::BufferCreateInfo bufferInfo;
            bufferInfo.size = bufferSize;
            bufferInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;
            bufferInfo.sharingMode = vk::SharingMode::eExclusive;

            m_buffer = m_ctx->getDevice().createBuffer(bufferInfo);

            vk::MemoryRequirements memRequirements = m_ctx->getDevice().getBufferMemoryRequirements(m_buffer);

            vk::MemoryAllocateInfo allocInfo;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = m_ctx->findMemoryType(
                memRequirements.memoryTypeBits,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
            );

            m_memory = m_ctx->getDevice().allocateMemory(allocInfo);
            m_ctx->getDevice().bindBufferMemory(m_buffer, m_memory, 0);

            m_isCreated = true;

            if (m_indices != nullptr) {
                mapBuffer(m_indices, static_cast<uint32_t>(m_size * m_stride));
            }
        }

        void VK_IndexBuffer::mapBuffer(void* data, uint32_t size)
        {
            if (!m_isCreated || !data || size == 0) {
                return;
            }

            void* mappedData = m_ctx->getDevice().mapMemory(m_memory, 0, size);
            memcpy(mappedData, data, size);
            m_ctx->getDevice().unmapMemory(m_memory);
        }

        void VK_IndexBuffer::destroy()
        {
            if (m_isCreated) {
                m_ctx->getDevice().waitIdle();

                if (m_buffer) {
                    m_ctx->getDevice().destroyBuffer(m_buffer);
                    m_buffer = vk::Buffer();
                }

                if (m_memory) {
                    m_ctx->getDevice().freeMemory(m_memory);
                    m_memory = vk::DeviceMemory();
                }

                m_isCreated = false;
            }
        }

        void VK_IndexBuffer::bind(CommandBuffer* cmdBuffer)
        {
            auto cmd = static_cast<VK_CommandBuffer*>(cmdBuffer);
            if (m_isCreated && m_buffer) {
                cmd->commandBuffer().bindIndexBuffer(m_buffer, 0, getIndexType());
            }
        }

        vk::IndexType VK_IndexBuffer::getIndexType() const
        {
            if (m_stride == 2) {
                return vk::IndexType::eUint16;
            } else {
                return vk::IndexType::eUint32;
            }
        }
    }
}