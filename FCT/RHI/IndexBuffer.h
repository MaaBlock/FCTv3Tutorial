//
// Created by Administrator on 2025/4/10.
//

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H
namespace FCT
{
    namespace RHI
    {
        class IndexBuffer
        {
        public:
            IndexBuffer()
            {
                m_indices = nullptr;
            }
            virtual ~IndexBuffer() {}
            template <typename T>
            void indexBuffer(std::vector<T>& indices)
            {
                static_assert(std::is_integral<T>::value, "index must be integral type");
                static_assert(std::is_unsigned<T>::value, "index must be unsigned type");
                m_indices = indices.data();
                m_size = indices.size();
                m_stride = sizeof(T);
            }

            void updataBuffer()
            {
                if (m_indices)
                {
                    //todo: 根据内存类型选择更新数据方式
                    mapBuffer(m_indices, m_size * m_stride);
                }
            }
            virtual void create() = 0;
            virtual void mapBuffer(void* data, uint32_t size) = 0;
            virtual void bind(CommandBuffer* buffer) = 0;
        protected:
            void* m_indices;
            size_t m_size;
            size_t m_stride;
        };
    }
}


#endif //INDEXBUFFER_H
