#include "../FCTAPI.h"

namespace FCT
{
    namespace RHI
    {
        VK_ConstBuffer::VK_ConstBuffer(VK_Context* ctx)
        {
            m_ctx = ctx;
            m_buffer = nullptr;
            m_frameNeedsUpdate.clear();
        }

        void VK_ConstBuffer::mapData()
        {
            if (!m_buffer) {
                return;
            }
/*
            if (!m_buffer->isDirty()) {
                return;
            }
*/
            const void* data = m_buffer->getData();
            size_t size = m_buffer->getSize();

            if (!data || size == 0) {
                return;
            }

            uint32_t maxFrames = m_ctx->maxFrameInFlight();
            size_t copySize = std::min(size, static_cast<size_t>(m_bufferInfos[0].range));
            UpdateFrequency frequency = m_layout.getUpdateFrequency();

            switch (frequency) {
            case UpdateFrequency::PerFrame:
            case UpdateFrequency::Dynamic:
                {
                    uint32_t frameIndex = m_ctx->currentFrameIndex();
                    if (frameIndex < m_mappedData.size() && m_mappedData[frameIndex]) {
                        memcpy(m_mappedData[frameIndex], data, copySize);
                    }
                    break;
                }

            case UpdateFrequency::Static:
            case UpdateFrequency::PerObject:
                {
                    // 对于静态或每对象数据，更新当前帧并标记其他帧需要更新
                    uint32_t currentFrame = m_ctx->currentFrameIndex();

                    // 更新当前帧
                    if (currentFrame < m_mappedData.size() && m_mappedData[currentFrame]) {
                        memcpy(m_mappedData[currentFrame], data, copySize);
                    }

                    // 标记其他帧需要更新
                    if (m_frameNeedsUpdate.size() < maxFrames) {
                        m_frameNeedsUpdate.resize(maxFrames, false);
                    }

                    for (uint32_t i = 0; i < maxFrames; i++) {
                        if (i != currentFrame) {
                            m_frameNeedsUpdate[i] = true;
                        }
                    }

                    break;
                }
            }

            m_buffer->clearDirty();
        }

        void VK_ConstBuffer::checkAndUpdateCurrentFrame()
        {
            if (!m_buffer) {
                return;
            }

            uint32_t currentFrame = m_ctx->currentFrameIndex();

            if (currentFrame < m_frameNeedsUpdate.size() && m_frameNeedsUpdate[currentFrame]) {
                if (currentFrame < m_mappedData.size() && m_mappedData[currentFrame]) {
                    const void* data = m_buffer->getData();
                    size_t size = m_buffer->getSize();

                    if (data && size > 0) {
                        size_t copySize = std::min(size, static_cast<size_t>(m_bufferInfos[currentFrame].range));
                        memcpy(m_mappedData[currentFrame], data, copySize);
                    }
                }

                m_frameNeedsUpdate[currentFrame] = false;
            }
        }

        void VK_ConstBuffer::create()
        {
            vk::Device device = m_ctx->getDevice();
            uint32_t maxFramesInFlight = m_ctx->maxFrameInFlight();

            vk::DeviceSize bufferSize = m_layout.getTotalSize();
            if (bufferSize == 0) {
                bufferSize = 256;
            }

            m_buffers.resize(maxFramesInFlight);
            m_memories.resize(maxFramesInFlight);
            m_mappedData.resize(maxFramesInFlight, nullptr);
            m_frameNeedsUpdate.resize(maxFramesInFlight, false);

            for (uint32_t i = 0; i < maxFramesInFlight; i++) {
                try {
                    vk::BufferCreateInfo bufferInfo;
                    bufferInfo.size = bufferSize;
                    bufferInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
                    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

                    m_buffers[i] = device.createBuffer(bufferInfo);

                    vk::MemoryRequirements memRequirements = device.getBufferMemoryRequirements(m_buffers[i]);

                    vk::MemoryAllocateInfo allocInfo;
                    allocInfo.allocationSize = memRequirements.size;
                    allocInfo.memoryTypeIndex = m_ctx->findMemoryType(
                        memRequirements.memoryTypeBits,
                        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                    );

                    m_memories[i] = device.allocateMemory(allocInfo);

                    device.bindBufferMemory(m_buffers[i], m_memories[i], 0);

                    m_mappedData[i] = device.mapMemory(m_memories[i], 0, bufferSize);

                    memset(m_mappedData[i], 0, static_cast<size_t>(bufferSize));
                }
                catch (const vk::SystemError& e) {
                    throw std::runtime_error("Failed to create uniform buffer: " + std::string(e.what()));
                }
            }

            m_bufferInfos.resize(maxFramesInFlight);
            for (uint32_t i = 0; i < maxFramesInFlight; i++) {
                m_bufferInfos[i].buffer = m_buffers[i];
                m_bufferInfos[i].offset = 0;
                m_bufferInfos[i].range = bufferSize;
            }
        }
    }
}