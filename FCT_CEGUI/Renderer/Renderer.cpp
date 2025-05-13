#include "../headers.h"
namespace FCT {
    CEGUI::RenderTarget& CEGUIRenderer::getDefaultRenderTarget()
    {
    }

    CEGUI::GeometryBuffer& CEGUIRenderer::createGeometryBufferColoured(
        CEGUI::RefCounted<CEGUI::RenderMaterial> renderMaterial)
    {
        CEGUIGeometryBuffer* geom_buffer = new CEGUIGeometryBuffer(*this, renderMaterial);

        geom_buffer->addVertexAttribute(CEGUI::VertexAttributeType::Position0);
        geom_buffer->addVertexAttribute(CEGUI::VertexAttributeType::Colour0);
        geom_buffer->finaliseVertexAttributes();

        addGeometryBuffer(*geom_buffer);
        return *geom_buffer;
    }

    CEGUI::GeometryBuffer& CEGUIRenderer::createGeometryBufferTextured(
        CEGUI::RefCounted<CEGUI::RenderMaterial> renderMaterial)
    {
        CEGUIGeometryBuffer* geom_buffer = new CEGUIGeometryBuffer(*this, renderMaterial);

        geom_buffer->addVertexAttribute(CEGUI::VertexAttributeType::Position0);
        geom_buffer->addVertexAttribute(CEGUI::VertexAttributeType::Colour0);
        geom_buffer->addVertexAttribute(CEGUI::VertexAttributeType::TexCoord0);
        geom_buffer->finaliseVertexAttributes();

        addGeometryBuffer(*geom_buffer);
        return *geom_buffer;
    }
}
