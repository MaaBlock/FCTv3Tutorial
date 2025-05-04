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
                m_updateFrequency = UpdateFrequency::Static;
            }
            void vertexBuffer(FCT::VertexBuffer* vertexBuffer)
            {
                m_vertexBuffer = vertexBuffer;
                if (m_size < vertexBuffer->getVertexCount())
                {
                    m_size = vertexBuffer->getVertexCount();
                }
                m_stride = vertexBuffer->getStride();
            }
            void updateFrequency(UpdateFrequency updateFrequency)
            {
                m_updateFrequency = updateFrequency;
            }
            void updataBuffer()
            {
                if (m_vertexBuffer)
                {
                    //todo: 根据内存类型选择更新数据方式
                    mapBuffer(m_vertexBuffer->getData(), m_vertexBuffer->getDataSize());
                }
            }
            void reserve(size_t size)
            {
                m_size = size;
            }

            virtual void create() = 0;
            virtual void mapBuffer(void* data, uint32_t size) = 0;
            virtual void bind(CommandBuffer* srcCmd, uint32_t slot = 0,uint32_t offset = 0) = 0;
            virtual void resize(uint32_t newSize) = 0;
        protected:
            UpdateFrequency m_updateFrequency;
            FCT::VertexBuffer* m_vertexBuffer;
            size_t m_size;
            size_t m_stride;
        };
    }
}
#endif //FCT_RHI_VERTEXBUFFER_H
