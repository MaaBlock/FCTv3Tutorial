//
// Created by Administrator on 2025/4/9.
//
#include "../Context/Vertex.h"
#ifndef FCT_RHI_VERTEXBUFFER_H
#define FCT_RHI_VERTEXBUFFER_H
namespace FCT
{
    namespace RHI
    {
        class VertexBuffer
        {
        public:
            VertexBuffer()
            {
                m_vertexBuffer = nullptr;
                m_size = 0;
                m_stride = 0;
            }
            void vertexBuffer(FCT::VertexBuffer* vertexBuffer)
            {
                m_vertexBuffer = vertexBuffer;
                m_size = vertexBuffer->getVertexCount();
                m_stride = vertexBuffer->getStride();
            }
            void updataBuffer()
            {
                if (m_vertexBuffer)
                {
                    //todo: 根据内存类型选择更新数据方式
                    mapBuffer(m_vertexBuffer->getData(), m_size * m_stride);
                }
            }
            virtual void create() = 0;
            virtual void mapBuffer(void* data, uint32_t size) = 0;
            virtual void bind(CommandBuffer* srcCmd, uint32_t slot = 0,uint32_t offset = 0) = 0;
        protected:
            FCT::VertexBuffer* m_vertexBuffer;
            size_t m_size;
            size_t m_stride;
        };
    }
}
#endif //FCT_RHI_VERTEXBUFFER_H
