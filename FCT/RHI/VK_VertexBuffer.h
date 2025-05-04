//
// Created by Administrator on 2025/4/9.
//
#include "./VertexBuffer.h"
#ifndef VK_VERTEXBUFFER_H
#define VK_VERTEXBUFFER_H

namespace FCT
{
    namespace RHI
    {

        class VK_VertexBuffer : public VertexBuffer{
        public:
            VK_VertexBuffer(VK_Context* ctx);
            ~VK_VertexBuffer();

            void createSingleBuffer(vk::Buffer& buffer, vk::DeviceMemory& memory);

            void create() override;

            void mapBuffer(void* data, uint32_t size) override;
            void mapBuffer(void* data, uint32_t size, uint32_t frameIndex);

            void bind(CommandBuffer* srcCmd, uint32_t slot, uint32_t offset) override;

            void resize(uint32_t newSize);

            void resizeBufferIfNeeded(uint32_t frameIndex);

        protected:
            std::vector<vk::Buffer> m_buffers;
            std::vector<vk::DeviceMemory> m_memories;

            std::vector<uint8_t> m_dirtyFrames;

            VK_Context* m_ctx;

            bool m_pendingResize = false;
            uint32_t m_newSize = 0;

            vk::Buffer m_buffer;
            vk::DeviceMemory m_memory;
        };
    }
}

#endif //VK_VERTEXBUFFER_H