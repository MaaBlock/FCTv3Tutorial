//
// Created by Administrator on 2025/4/10.
//

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H
namespace FCT
{
    namespace RHI
    {
        class IIndexData
        {
        public:
            virtual ~IIndexData() = default;
            virtual void* getData() = 0;
            virtual size_t getSize() = 0;
            virtual size_t getStride() = 0;
        };
        template <typename T>
       class IndexData : public IIndexData
        {
        public:
            IndexData(std::vector<T>& indices) : m_indices(indices) {}

            void* getData() override { return m_indices.data(); }
            size_t getSize() override { return m_indices.size(); }
            size_t getStride() override { return sizeof(T); }

        private:
            std::vector<T>& m_indices;
        };

        class IndexBuffer
        {
        public:
            IndexBuffer()
            {
                m_indexData = nullptr;
                m_updateFrequency = UpdateFrequency::Static;
                m_size = 0;
            }
            virtual ~IndexBuffer()
            {
                if (m_indexData)
                {
                    delete m_indexData;
                    m_indexData = nullptr;
                }
            }
            template <typename T>
            void indexBuffer(std::vector<T>& indices)
            {
                static_assert(std::is_integral<T>::value, "index must be integral type");
                static_assert(std::is_unsigned<T>::value, "index must be unsigned type");
                m_indexData = new IndexData<T>(indices);
                if (m_size < indices.size())
                {
                    m_size = indices.size();
                }
                m_stride = sizeof(T);
            }
            void reserve(size_t size)
            {
                m_size = size;
            }
            void updateFrequency(UpdateFrequency frequency)
            {
                m_updateFrequency = frequency;
            }
            void updataBuffer()
            {
                if (m_indexData)
                {
                    //todo: 根据内存类型选择更新数据方式
                    mapBuffer(m_indexData->getData(), m_indexData->getSize() * m_indexData->getStride());
                }
            }
            virtual void create() = 0;
            virtual void mapBuffer(void* data, uint32_t size) = 0;
            virtual void bind(CommandBuffer* buffer) = 0;
            virtual void resize(size_t size) = 0;
        protected:
            size_t m_size;
            size_t m_stride;
            IIndexData* m_indexData;
            UpdateFrequency m_updateFrequency;
        };
    }
}


#endif //INDEXBUFFER_H
