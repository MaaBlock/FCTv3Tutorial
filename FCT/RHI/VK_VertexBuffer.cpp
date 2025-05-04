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
            m_updateFrequency = UpdateFrequency::Static;
            m_pendingResize = false;
            m_newSize = 0;
        }

        VK_VertexBuffer::~VK_VertexBuffer()
        {
            for (size_t i = 0; i < m_buffers.size(); ++i)
            {
                if (m_buffers[i])
                {
                    m_ctx->getDevice().destroyBuffer(m_buffers[i]);
                }

                if (m_memories[i])
                {
                    m_ctx->getDevice().freeMemory(m_memories[i]);
                }
            }
        }

        void VK_VertexBuffer::createSingleBuffer(vk::Buffer& buffer, vk::DeviceMemory& memory)
        {
            vk::BufferCreateInfo bufferInfo{};
            bufferInfo.size = m_size * m_stride;
            bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
            bufferInfo.sharingMode = vk::SharingMode::eExclusive;
            buffer = m_ctx->getDevice().createBuffer(bufferInfo);
            vk::MemoryRequirements memRequirements = m_ctx->getDevice().getBufferMemoryRequirements(buffer);
            vk::MemoryAllocateInfo allocInfo{};
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = m_ctx->findMemoryType(memRequirements.memoryTypeBits,
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent);
            memory = m_ctx->getDevice().allocateMemory(allocInfo);
            m_ctx->getDevice().bindBufferMemory(buffer, memory, 0);
        }

        void VK_VertexBuffer::resize(uint32_t newSize)
        {
            if (newSize == m_size)
                return;

            m_newSize = newSize;
            m_pendingResize = true;

            if (m_updateFrequency == UpdateFrequency::Static)
            {
                m_dirtyFrames.resize(1, true);
            }
            else
            {
                uint32_t frameCount = m_ctx->maxFrameInFlight();
                m_dirtyFrames.resize(frameCount, true);
            }

            if (m_updateFrequency == UpdateFrequency::Static)
            {
                resizeBufferIfNeeded(0);
            }
            else
            {
                uint32_t currentFrame = m_ctx->currentFrameIndex();
                resizeBufferIfNeeded(currentFrame);
            }
        }

        void VK_VertexBuffer::resizeBufferIfNeeded(uint32_t frameIndex)
        {
            if (frameIndex >= m_buffers.size() || frameIndex >= m_dirtyFrames.size())
                return;

            if (!m_dirtyFrames[frameIndex])
                return;

            vk::Buffer oldBuffer = m_buffers[frameIndex];
            vk::DeviceMemory oldMemory = m_memories[frameIndex];

            m_size = m_newSize;

            createSingleBuffer(m_buffers[frameIndex], m_memories[frameIndex]);

            if (oldBuffer)
            {
                m_ctx->getDevice().destroyBuffer(oldBuffer);
            }

            if (oldMemory)
            {
                m_ctx->getDevice().freeMemory(oldMemory);
            }

            m_dirtyFrames[frameIndex] = false;

            bool allResized = true;
            for (bool dirty : m_dirtyFrames)
            {
                if (dirty)
                {
                    allResized = false;
                    break;
                }
            }

            if (allResized)
            {
                m_pendingResize = false;
            }
        }

        void VK_VertexBuffer::create()
        {
            if (m_updateFrequency == UpdateFrequency::Static)
            {
                m_buffers.resize(1);
                m_memories.resize(1);
                m_dirtyFrames.resize(1, false);
                createSingleBuffer(m_buffers[0], m_memories[0]);
            }
            else
            {
                uint32_t frameCount = m_ctx->maxFrameInFlight();
                m_buffers.resize(frameCount);
                m_memories.resize(frameCount);
                m_dirtyFrames.resize(frameCount, false);

                for (uint32_t i = 0; i < frameCount; ++i)
                {
                    createSingleBuffer(m_buffers[i], m_memories[i]);
                }
            }

            m_pendingResize = false;
        }

        void VK_VertexBuffer::mapBuffer(void* data, uint32_t size)
        {
            if (m_updateFrequency == UpdateFrequency::Static)
            {
                resizeBufferIfNeeded(0);

                void* mappedData = m_ctx->getDevice().mapMemory(m_memories[0], 0, size, vk::MemoryMapFlags());
                memcpy(mappedData, data, size);
                m_ctx->getDevice().unmapMemory(m_memories[0]);
            }
            else
            {
                uint32_t currentFrame = m_ctx->currentFrameIndex();

                resizeBufferIfNeeded(currentFrame);

                void* mappedData = m_ctx->getDevice().mapMemory(m_memories[currentFrame], 0, size, vk::MemoryMapFlags());
                memcpy(mappedData, data, size);
                m_ctx->getDevice().unmapMemory(m_memories[currentFrame]);
            }
        }

        void VK_VertexBuffer::mapBuffer(void* data, uint32_t size, uint32_t frameIndex)
        {
            if (m_updateFrequency == UpdateFrequency::Static)
            {
                resizeBufferIfNeeded(0);

                void* mappedData = m_ctx->getDevice().mapMemory(m_memories[0], 0, size, vk::MemoryMapFlags());
                memcpy(mappedData, data, size);
                m_ctx->getDevice().unmapMemory(m_memories[0]);
            }
            else
            {
                uint32_t index = frameIndex % m_ctx->maxFrameInFlight();

                resizeBufferIfNeeded(index);

                void* mappedData = m_ctx->getDevice().mapMemory(m_memories[index], 0, size, vk::MemoryMapFlags());
                memcpy(mappedData, data, size);
                m_ctx->getDevice().unmapMemory(m_memories[index]);
            }
        }

        void VK_VertexBuffer::bind(CommandBuffer* srcCmd, uint32_t slot, uint32_t srcOffset)
        {
            VK_CommandBuffer* cmd = static_cast<VK_CommandBuffer*>(srcCmd);
            vk::DeviceSize offset = static_cast<vk::DeviceSize>(srcOffset);

            if (m_updateFrequency == UpdateFrequency::Static)
            {
                resizeBufferIfNeeded(0);

                cmd->commandBuffer().bindVertexBuffers(slot, 1, &m_buffers[0], &offset);
            }
            else
            {
                uint32_t currentFrame = m_ctx->currentSubmitFrameIndex();

                resizeBufferIfNeeded(currentFrame);

                cmd->commandBuffer().bindVertexBuffers(slot, 1, &m_buffers[currentFrame], &offset);
            }
        }
    }
}