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
            void mapBuffer(void* data, uint32_t size) override;

            void destroy();
            void bind(CommandBuffer* cmdBuffer);

            vk::Buffer getBuffer() const { return m_buffer; }
            vk::IndexType getIndexType() const;

        private:
            VK_Context* m_ctx;
            vk::Buffer m_buffer;
            vk::DeviceMemory m_memory;
            bool m_isCreated = false;
        };
    }
}
#endif //VK_INDEXBUFFER_H