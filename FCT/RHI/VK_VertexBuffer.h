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
            void create();
            void mapBuffer(void* data, uint32_t size) override;
            void bind(CommandBuffer* srcCmd, uint32_t slot,uint32_t offset) override;
        protected:
            vk::Buffer m_buffer;
            VK_Context* m_ctx;
            vk::DeviceMemory m_memory;
        };
    }

}

#endif //VK_VERTEXBUFFER_H
