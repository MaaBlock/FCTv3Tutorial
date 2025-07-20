//
// Created by Administrator on 2025/5/12.
//

#ifndef CEGUIGEOMETRYBUFFER_H
#define CEGUIGEOMETRYBUFFER_H
#include "../ThirdParty.h"
#include "./Renderer.h"
namespace FCT
{
    class CEGUIGeometryBuffer : public CEGUI::GeometryBuffer {
    protected:
        CEGUIRenderer& m_owner;
        VertexLayout m_vertexLayout;
        mutable DynamicMesh<uint32_t>* m_mesh;
        mutable bool m_geometryDirty;
    public:
        CEGUIGeometryBuffer(CEGUIRenderer& renderer, CEGUI::RefCounted<CEGUI::RenderMaterial> renderMaterial);
        void draw(uint32_t drawModeMask) const override;
        void finaliseVertexAttributes();
    protected:
        virtual void onGeometryChanged() override { m_geometryDirty = true; }
        void uploadMesh() const;
    };
} // FCT

#endif //CEGUIGEOMETRYBUFFER_H
