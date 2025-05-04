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
            uint32_t bufferCount = (m_updateFrequency == UpdateFrequency::PerFrame) ?
                                   m_ctx->maxFrameInFlight() : 1;

            m_buffers.resize(bufferCount);
            m_memories.resize(bufferCount);
            m_dirtyFlags.resize(bufferCount, 0);

            for (uint32_t i = 0; i < bufferCount; i++) {
                vk::BufferCreateInfo bufferInfo;
                bufferInfo.size = bufferSize;
                bufferInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;
                bufferInfo.sharingMode = vk::SharingMode::eExclusive;

                m_buffers[i] = m_ctx->getDevice().createBuffer(bufferInfo);

                vk::MemoryRequirements memRequirements = m_ctx->getDevice().getBufferMemoryRequirements(m_buffers[i]);

                vk::MemoryAllocateInfo allocInfo;
                allocInfo.allocationSize = memRequirements.size;
                allocInfo.memoryTypeIndex = m_ctx->findMemoryType(
                    memRequirements.memoryTypeBits,
                    vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                );

                m_memories[i] = m_ctx->getDevice().allocateMemory(allocInfo);
                m_ctx->getDevice().bindBufferMemory(m_buffers[i], m_memories[i], 0);
            }

            m_isCreated = true;

            updataBuffer();
        }
        void VK_IndexBuffer::mapBuffer(void* data, uint32_t size, uint32_t frameIndex)
        {
            if (!m_isCreated || !data || size == 0) {
                return;
            }

            if (m_updateFrequency == UpdateFrequency::PerFrame) {
                if (frameIndex >= m_buffers.size()) {
                    frameIndex = frameIndex % m_buffers.size();
                }

                checkAndResizeIfNeeded(frameIndex);

            } else {
                frameIndex = 0;
            }

            void* mappedData = m_ctx->getDevice().mapMemory(m_memories[frameIndex], 0, size);
            memcpy(mappedData, data, size);
            m_ctx->getDevice().unmapMemory(m_memories[frameIndex]);
        }

        void VK_IndexBuffer::mapBuffer(void* data, uint32_t size)
        {
            uint32_t frameIndex = 0;
            if (m_updateFrequency == UpdateFrequency::PerFrame) {
                frameIndex = m_ctx->currentFrameIndex();
            }
            mapBuffer(data, size, frameIndex);
        }

        void VK_IndexBuffer::destroy()
        {
            if (m_isCreated) {
                m_ctx->getDevice().waitIdle();

                for (size_t i = 0; i < m_buffers.size(); i++) {
                    if (m_buffers[i]) {
                        m_ctx->getDevice().destroyBuffer(m_buffers[i]);
                    }

                    if (m_memories[i]) {
                        m_ctx->getDevice().freeMemory(m_memories[i]);
                    }
                }

                m_buffers.clear();
                m_memories.clear();

                m_isCreated = false;
            }
        }
        void VK_IndexBuffer::bind(CommandBuffer* cmdBuffer)
        {
            auto cmd = static_cast<VK_CommandBuffer*>(cmdBuffer);
            if (m_isCreated) {
                uint32_t frameIndex = 0;
                if (m_updateFrequency == UpdateFrequency::PerFrame) {
                    frameIndex = m_ctx->currentSubmitFrameIndex();
                    checkAndResizeIfNeeded(frameIndex);
                }

                if (frameIndex < m_buffers.size() && m_buffers[frameIndex]) {
                    cmd->commandBuffer().bindIndexBuffer(m_buffers[frameIndex], 0, getIndexType());
                }
            }
        }
        void VK_IndexBuffer::checkAndResizeIfNeeded(uint32_t frameIndex)
        {
            if (frameIndex < m_dirtyFlags.size() && m_dirtyFlags[frameIndex]) {
                uint32_t currentFrame = m_ctx->currentFrameIndex();

                if (currentFrame != frameIndex && currentFrame < m_buffers.size()) {
                    resizeBuffer(m_size, frameIndex);

                    m_dirtyFlags[frameIndex] = 0;
                }
            }
        }
        void VK_IndexBuffer::resizeBuffer(size_t size, uint32_t frameIndex)
        {
            if (size == m_size || !m_isCreated) {
                return;
            }

            if (frameIndex >= m_buffers.size()) {
                frameIndex = frameIndex % m_buffers.size();
            }

            size_t oldSize = m_size;
            uint32_t oldBufferSize = static_cast<uint32_t>(oldSize * m_stride);
            uint32_t newBufferSize = static_cast<uint32_t>(size * m_stride);

            void* tempData = nullptr;
            uint32_t copySize = static_cast<uint32_t>(std::min(oldSize, size) * m_stride);

            if (copySize > 0) {
                tempData = malloc(copySize);
                if (tempData) {
                    void* srcData = m_ctx->getDevice().mapMemory(m_memories[frameIndex], 0, copySize);
                    memcpy(tempData, srcData, copySize);
                    m_ctx->getDevice().unmapMemory(m_memories[frameIndex]);
                }
            }

            m_size = size;

            if (m_buffers[frameIndex]) {
                m_ctx->getDevice().destroyBuffer(m_buffers[frameIndex]);
            }

            if (m_memories[frameIndex]) {
                m_ctx->getDevice().freeMemory(m_memories[frameIndex]);
            }

            vk::BufferCreateInfo bufferInfo;
            bufferInfo.size = newBufferSize;
            bufferInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;
            bufferInfo.sharingMode = vk::SharingMode::eExclusive;

            m_buffers[frameIndex] = m_ctx->getDevice().createBuffer(bufferInfo);

            vk::MemoryRequirements memRequirements = m_ctx->getDevice().getBufferMemoryRequirements(m_buffers[frameIndex]);

            vk::MemoryAllocateInfo allocInfo;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = m_ctx->findMemoryType(
                memRequirements.memoryTypeBits,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
            );

            m_memories[frameIndex] = m_ctx->getDevice().allocateMemory(allocInfo);
            m_ctx->getDevice().bindBufferMemory(m_buffers[frameIndex], m_memories[frameIndex], 0);

            if (tempData && copySize > 0) {
                void* mappedData = m_ctx->getDevice().mapMemory(m_memories[frameIndex], 0, copySize);
                memcpy(mappedData, tempData, copySize);
                m_ctx->getDevice().unmapMemory(m_memories[frameIndex]);
                free(tempData);
            }

            if (frameIndex < m_dirtyFlags.size()) {
                m_dirtyFlags[frameIndex] = 0;
            }
        }

        void VK_IndexBuffer::resize(size_t size)
        {
            if (size == m_size) {
                return;
            }

            if (m_updateFrequency == UpdateFrequency::PerFrame) {
                uint32_t currentFrame = m_ctx->currentFrameIndex();

                resizeBuffer(size, currentFrame);

                for (uint32_t i = 0; i < m_dirtyFlags.size(); i++) {
                    if (i != currentFrame) {
                        m_dirtyFlags[i] = 1;
                    }
                }
            } else {
                resizeBuffer(size, 0);
            }
        }
        vk::IndexType VK_IndexBuffer::getIndexType() const
        {
            switch (m_stride)
            {
            case 1:
                return vk::IndexType::eUint8;
            case 2:
                return vk::IndexType::eUint16;
            case 4:
                return vk::IndexType::eUint32;
            default:
                return vk::IndexType::eUint32;
            }
        }
    }
}