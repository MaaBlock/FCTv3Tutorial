//
// Created by Administrator on 2025/5/12.
//

#include "CEGUIGeometryBuffer.h"

namespace FCT {
    CEGUIGeometryBuffer::CEGUIGeometryBuffer(CEGUIRenderer& renderer,
        CEGUI::RefCounted<CEGUI::RenderMaterial> renderMaterial)
    : GeometryBuffer(renderMaterial)
    , m_owner(renderer)
    , m_mesh(nullptr)
    , m_geometryDirty(true)
    {
    }

    void CEGUIGeometryBuffer::draw(uint32_t drawModeMask) const
    {
        CEGUI_UNUSED(drawModeMask);
        if (d_vertexData.empty())
            return;
        if (m_geometryDirty)
            uploadMesh();
    }

    void CEGUIGeometryBuffer::finaliseVertexAttributes()
    {
        for (auto &attrib : d_vertexAttributes)
        {
            switch (attrib)
            {
            case CEGUI::VertexAttributeType::Position0:
                m_vertexLayout.addElement(VertexElement{VtxType::Position3f});
                break;
            case CEGUI::VertexAttributeType::Colour0:
                m_vertexLayout.addElement(VertexElement{VtxType::Color4f});
                break;
            case CEGUI::VertexAttributeType::TexCoord0:
                m_vertexLayout.addElement(VertexElement{VtxType::TexCoord2f});
                break;
            }
        }
        if (m_mesh)
        {
            FCT_DELETE(m_mesh);
            m_mesh = nullptr;
        }
        m_mesh = new DynamicMesh<uint32_t>(m_owner.ctx(), m_vertexLayout);

        m_mesh->create();
    }

    void CEGUIGeometryBuffer::uploadMesh() const
    {
        m_geometryDirty = false;

    }
} // FCT