//
// Created by Administrator on 2025/4/22.
//
#include "./Vertex.h"
#include "../RHI/IndexBuffer.h"
#include "../RHI/VertexBuffer.h"
#ifndef FCT_MESH_H
#define FCT_MESH_H
namespace FCT
{
    template<typename IndexType = uint16_t>
    class Mesh
    {
    public:
        Mesh(Context* context, const VertexLayout& layout)
            : m_ctx(context), m_vertexLayout(layout), m_gpuIndex(nullptr), m_gpuVertex(nullptr)
        {
            m_cpuVertex = new VertexBuffer(layout);
        }

        ~Mesh()
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
}
#endif //MESH_H
