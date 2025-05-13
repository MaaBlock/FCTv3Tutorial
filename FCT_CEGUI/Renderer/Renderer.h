#ifndef FCT_CEGUI_RENDERER_H
#define FCT_CEGUI_RENDERER_H
#include "../ThirdParty.h"
namespace FCT
{
    class CEGUIRenderer : public CEGUI::Renderer
    {
    protected:
        Context* m_ctx;
    public:
        CEGUI::RenderTarget& getDefaultRenderTarget();
        CEGUI::GeometryBuffer& createGeometryBufferColoured(CEGUI::RefCounted<CEGUI::RenderMaterial> renderMaterial) override;
        CEGUI::GeometryBuffer& createGeometryBufferTextured(CEGUI::RefCounted<CEGUI::RenderMaterial> renderMaterial) override;
        Context* ctx() const { return m_ctx; }
    };
}

#endif //RENDERER_H
