//
// Created by Administrator on 2025/4/10.
//
#ifndef VK_INDEXBUFFER_H
#define VK_INDEXBUFFER_H

#include "./IndexBuffer.h"

namespace FCT
{
    namespace RHI
    {
        class VK_IndexBuffer : public IndexBuffer
        {
        public:
            VK_IndexBuffer(VK_Context* ctx);
            ~VK_IndexBuffer();

            void create() override;
            void mapBuffer(void* data, uint32_t size, uint32_t frameIndex);
            void mapBuffer(void* data, uint32_t size) override;

            void destroy();
            void bind(CommandBuffer* cmdBuffer);
            void checkAndResizeIfNeeded(uint32_t frameIndex);
            vk::IndexType getIndexType() const;
            void resizeBuffer(size_t size, uint32_t frameIndex);
            void resize(size_t size) override;
        private:
            VK_Context* m_ctx;
            std::vector<vk::Buffer> m_buffers;
            std::vector<vk::DeviceMemory> m_memories;
            std::vector<uint8_t> m_dirtyFlags;
            bool m_isCreated = false;
        };
    }
}
#endif //VK_INDEXBUFFER_H