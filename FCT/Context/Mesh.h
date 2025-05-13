//
// Created by Administrator on 2025/4/22.
//
#ifndef FCT_MESH_H
#define FCT_MESH_H
#include "../ThirdParty.h"
#include "./Vertex.h"
#include "../RHI/IndexBuffer.h"
#include "../RHI/VertexBuffer.h"
#include "../RHI/CommandBuffer.h"
namespace FCT
{
    template<typename IndexType = uint16_t>
    class StaticMesh
    {
    public:
        StaticMesh(Context* context, const VertexLayout& layout)
            : m_ctx(context), m_vertexLayout(layout), m_gpuIndex(nullptr), m_gpuVertex(nullptr)
        {
            m_cpuVertex = new VertexBuffer(layout);
        }

        ~StaticMesh()
        {
            if (m_cpuVertex) {
                delete m_cpuVertex;
            }
            if (m_gpuVertex) {
                delete m_gpuVertex;
            }
            if (m_gpuIndex) {
                delete m_gpuIndex;
            }
        }

        template<typename... Args>
        void addVertex(Args&&... args)
        {
            m_cpuVertex->emplaceBack(std::forward<Args>(args)...);
        }

        void setIndices(const std::vector<IndexType>& indices)
        {
            m_cpuIndices = indices;
        }

        void create()
        {
            m_gpuVertex = m_ctx->createVertexBuffer();
            m_gpuVertex->vertexBuffer(m_cpuVertex);
            m_gpuVertex->create();
            m_gpuVertex->updataBuffer();

            if (!m_cpuIndices.empty()) {
                m_gpuIndex = m_ctx->createIndexBuffer();
                m_gpuIndex->indexBuffer(m_cpuIndices);
                m_gpuIndex->create();
                m_gpuIndex->updataBuffer();
            }
        }

        void bind(RHI::CommandBuffer* cmdBuf)
        {
            if (m_gpuVertex) {
                m_gpuVertex->bind(cmdBuf);
            }

            if (m_gpuIndex) {
                m_gpuIndex->bind(cmdBuf);
            }
        }

        void draw(RHI::CommandBuffer* cmdBuf, uint32_t instanceCount = 1)
        {
            if (m_gpuIndex) {
                cmdBuf->drawIndex(0, 0, static_cast<uint32_t>(m_cpuIndices.size()), instanceCount);
            } else {
                cmdBuf->draw(0, 0, m_cpuVertex->getVertexCount(), instanceCount);
            }
        }

        void updateVertexData()
        {
            if (m_gpuVertex) {
                m_gpuVertex->updataBuffer();
            }
        }

        void updateIndexData()
        {
            if (m_gpuIndex) {
                m_gpuIndex->updataBuffer();
            }
        }

        VertexBuffer* getVertexBuffer() const { return m_cpuVertex; }

        size_t getIndexCount() const { return m_cpuIndices.size(); }

        size_t getVertexCount() const { return m_cpuVertex ? m_cpuVertex->getVertexCount() : 0; }

    private:
        Context* m_ctx;
        VertexLayout m_vertexLayout;
        VertexBuffer* m_cpuVertex;
        RHI::VertexBuffer* m_gpuVertex;
        RHI::IndexBuffer* m_gpuIndex;
        std::vector<IndexType> m_cpuIndices;
    };
    template<typename IndexType = uint16_t>
    class DynamicMesh
    {
    public:
        DynamicMesh(Context* context, const VertexLayout& layout)
            : m_ctx(context), m_vertexLayout(layout), m_gpuIndex(nullptr), m_gpuVertex(nullptr)
        {
            m_cpuVertex = new VertexBuffer(layout);
        }

        ~DynamicMesh()
        {
            if (m_cpuVertex) {
                delete m_cpuVertex;
            }
            if (m_gpuVertex) {
                delete m_gpuVertex;
            }
            if (m_gpuIndex) {
                delete m_gpuIndex;
            }
        }

        template<typename... Args>
        void addVertex(Args&&... args)
        {
            if (m_gpuVertex && m_cpuVertex->getVertexCount() >= m_vertexCapacity) {
                size_t newCapacity = m_vertexCapacity == 0 ? 1 : m_vertexCapacity * 2;
                resizeVertexBuffer(newCapacity);
            }
            m_cpuVertex->emplaceBack(std::forward<Args>(args)...);
        }
        void addIndex(IndexType index)
        {
            if (m_gpuIndex && m_cpuIndices.size() >= m_indexCapacity) {
                size_t newCapacity = m_indexCapacity == 0 ? 1 : m_indexCapacity * 2;
                resizeIndexBuffer(newCapacity);
            }
            m_cpuIndices.push_back(index);
        }

        void addIndices(const std::vector<IndexType>& indices)
        {
            if (m_gpuIndex && (m_cpuIndices.size() + indices.size()) > m_indexCapacity) {
                size_t tmp = m_cpuIndices.size() + indices.size();
                size_t tmp2 = m_indexCapacity * 2;
                size_t newCapacity = std::max(tmp2, tmp);
                resizeIndexBuffer(newCapacity);
            }

            m_cpuIndices.insert(m_cpuIndices.end(), indices.begin(), indices.end());
        }

        void setIndices(const std::vector<IndexType>& indices)
        {
            m_cpuIndices = indices;
            m_indexCapacity = indices.size();

            if (m_gpuIndex) {
                m_gpuIndex->resize(m_indexCapacity);
            }
        }

        void reserveVertices(size_t capacity)
        {
            if (!m_gpuVertex) {
                m_vertexCapacity = capacity;
            }
        }

        void reserveIndices(size_t capacity)
        {
            if (!m_gpuIndex) {
                m_indexCapacity = capacity;
            }
        }

        void create()
        {
            m_gpuVertex = m_ctx->createVertexBuffer();
            m_gpuVertex->vertexBuffer(m_cpuVertex);
            m_gpuVertex->updateFrequency(UpdateFrequency::PerFrame);

            if (m_vertexCapacity > m_cpuVertex->getVertexCount()) {

            } else {
                m_vertexCapacity = m_cpuVertex->getVertexCount();
            }
            m_gpuVertex->reserve(m_vertexCapacity);

            m_gpuVertex->create();
            m_gpuVertex->updataBuffer();

            if (!m_cpuIndices.empty() || m_indexCapacity > 0) {
                m_gpuIndex = m_ctx->createIndexBuffer();
                m_gpuIndex->indexBuffer(m_cpuIndices);
                m_gpuIndex->updateFrequency(UpdateFrequency::PerFrame);

                if (m_indexCapacity > m_cpuIndices.size()) {
                } else {
                    m_indexCapacity = m_cpuIndices.size();
                }
                m_gpuIndex->reserve(m_indexCapacity);

                m_gpuIndex->create();
                m_gpuIndex->updataBuffer();
            }
        }

        void bind(RHI::CommandBuffer* cmdBuf)
        {
            if (m_gpuVertex) {
                m_gpuVertex->bind(cmdBuf);
            }

            if (m_gpuIndex) {
                m_gpuIndex->bind(cmdBuf);
            }
        }

        void draw(RHI::CommandBuffer* cmdBuf, uint32_t instanceCount = 1)
        {
            if (m_gpuIndex && !m_cpuIndices.empty()) {
                cmdBuf->drawIndex(0, 0, static_cast<uint32_t>(m_cpuIndices.size()), instanceCount);
            } else if (m_cpuVertex && m_cpuVertex->getVertexCount() > 0) {
                cmdBuf->draw(0, 0, m_cpuVertex->getVertexCount(), instanceCount);
            }
        }

        void clear()
        {
            if (m_cpuVertex) {
                m_cpuVertex->clear();
            }
            m_cpuIndices.clear();
        }

        void updateVertexData()
        {
            if (m_gpuVertex) {
                m_gpuVertex->updataBuffer();
            }
        }

        void updateIndexData()
        {
            if (m_gpuIndex) {
                m_gpuIndex->updataBuffer();
            }
        }
        void update()
        {
            updateVertexData();
            updateIndexData();
        }


        VertexBuffer* getVertexBuffer() const { return m_cpuVertex; }

        size_t getIndexCount() const { return m_cpuIndices.size(); }

        size_t getVertexCount() const { return m_cpuVertex ? m_cpuVertex->getVertexCount() : 0; }

        size_t getVertexCapacity() const { return m_vertexCapacity; }

        size_t getIndexCapacity() const { return m_indexCapacity; }
    private:
        void resizeVertexBuffer(size_t newCapacity)
        {
            if (m_gpuVertex) {
                m_gpuVertex->resize(newCapacity);
                m_vertexCapacity = newCapacity;
            }
        }

        void resizeIndexBuffer(size_t newCapacity)
        {
            if (m_gpuIndex) {
                m_gpuIndex->resize(newCapacity);
                m_indexCapacity = newCapacity;
            }
        }

    private:
        Context* m_ctx;
        VertexLayout m_vertexLayout;
        VertexBuffer* m_cpuVertex;
        RHI::VertexBuffer* m_gpuVertex;
        RHI::IndexBuffer* m_gpuIndex;
        std::vector<IndexType> m_cpuIndices;

        size_t m_vertexCapacity = 0;
        size_t m_indexCapacity = 0;
    };
}
#endif //MESH_H
